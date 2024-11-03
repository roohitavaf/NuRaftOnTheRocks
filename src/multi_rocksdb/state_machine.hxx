
#pragma once

#include "libnuraft/nuraft.hxx"
#include "../common/state_machine_interface.hxx"

#include <iostream>
#include <rocksdb/db.h>
#include <string>

using namespace nuraft;

namespace nuraft_on_the_rocks {

class MultiRocksDBStateMachine : public KeyValueStateMachine {
private:
    rocksdb::DB* db_;

public:
    MultiRocksDBStateMachine(int id) {
        std::string db_path = "MultiRocksDBStore" + std::to_string(id);
        rocksdb::Options options;
        options.create_if_missing = true;

        rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db_);
        if (!status.ok()) {
            std::cerr << "Unable to open RocksDB: " << status.ToString() << std::endl;
            exit(1);
        }
    }

    ~MultiRocksDBStateMachine() {
        db_->Close();
        delete db_;
    }
    
    bool getKey(std::string key, std::string& value) override {
        auto status = db_->Get(rocksdb::ReadOptions(), key, &value);
        if (!status.ok()) {
            return false;
        }
        return true;
    }

    bool scan(std::string startKey, std::string endKey, std::vector<std::pair<std::string, std::string>>& result) override {
        auto it(db_->NewIterator(rocksdb::ReadOptions()));
        for (it->Seek(startKey); it->Valid() && it->key().ToString() <= endKey; it->Next()) {
            result.emplace_back(it->key().ToString(), it->value().ToString());
        }   
        return true;
    }

    void applyPayload(op_payload payload) override {
        if (payload.value.empty()) {
            auto status = db_->Delete(rocksdb::WriteOptions(), payload.key);
            if (!status.ok()) {
                std::cerr << "Failed to apply payload. " << std::endl;
            }
        } else {
            auto status = db_->Put(rocksdb::WriteOptions(), payload.key, payload.value);
            if (!status.ok()) {
                std::cerr << "Failed to apply payload. " << std::endl;
            }
        }
    }
};
};

