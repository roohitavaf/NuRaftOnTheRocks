#include "../common/kv_server_interface.hxx"

#include <rocksdb/db.h>
#include <string>

namespace nuraft_on_the_rocks {
class SingleRocksDB final :  public KeyValueServer {
private:
    rocksdb::DB* db_;

public:
    SingleRocksDB() {
        std::string db_path = "SingleRocksDBStore";
        rocksdb::Options options;
        options.create_if_missing = true;

        rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db_);
        if (!status.ok()) {
            std::cerr << "Unable to open RocksDB: " << status.ToString() << std::endl;
            exit(1);
        }
    }

    ~SingleRocksDB() {
        db_->Close();
        delete db_;
    }

    int getKey(std::string key, std::string& value) {
        auto status = db_->Get(rocksdb::ReadOptions(), key, &value);
        return !status.ok();
    }

    int putKey(std::string key, std::string value) {
        auto status = db_->Put(rocksdb::WriteOptions(), key, value);
        return !status.ok();
    }

    int deleteKey(std::string key) {
        auto status = db_->Delete(rocksdb::WriteOptions(), key);
        return !status.ok(); 
    }

    int scanKey(std::string startKey, std::string endKey, std::vector<std::pair<std::string, std::string>>& keyValues) {
        auto it(db_->NewIterator(rocksdb::ReadOptions()));
        for (it->Seek(startKey); it->Valid() && it->key().ToString() <= endKey; it->Next()) {
            keyValues.emplace_back(it->key().ToString(), it->value().ToString());
        }
        return 0;
    }
};
}