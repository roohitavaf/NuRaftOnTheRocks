
#pragma once

#include "libnuraft/nuraft.hxx"
#include "../common/state_machine_interface.hxx"

#include <map>
#include <string>

using namespace nuraft;

namespace nuraft_on_the_rocks {

class MultiInMemoryStateMachine : public KeyValueStateMachine {
public:
    bool getKey(std::string key, std::string& value) override {
        auto it = kv_store.find(key);
        if (it == kv_store.end()) {
            return false;
        } 
        value = it->second;
        return true;
    }

    bool scan(std::string startKey, std::string endtKey, std::vector<std::pair<std::string, std::string>>& result) override {
        auto it = kv_store.lower_bound(startKey);
        while (it != kv_store.end() && it->first <= endtKey) {
            result.emplace_back(it->first, it->second);
            ++it;
        }
        return true;

    }

    virtual void applyPayload(op_payload payload) override {
        if (payload.value.empty()) {
            kv_store.erase(payload.key);
        } else {
            kv_store[payload.key] = payload.value;
        }
    }
private:
    // State machine state, i.e., the sorted key-value store
    std::map<std::string, std::string> kv_store;
};

};

