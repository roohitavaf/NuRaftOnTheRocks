#include "../common/kv_server_interface.hxx"

#include <string>
#include <map>

namespace nuraft_on_the_rocks {

class SingleInMemory final : public KeyValueServer {
private:
    std::map<std::string, std::string> map_;

public:
    int getKey(std::string key, std::string& value) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            value = it->second;
            return 0;
        } else {
            return -1;
        }
    }
    int putKey(std::string key, std::string value) {
        map_[key] = value;
        return 0;
    }
    int deleteKey(std::string key) {
        map_.erase(key);
        return 0;

    }
    int scanKey(std::string startKey, std::string endKey, std::vector<std::pair<std::string, std::string>>& keyValues) {
        auto it = map_.lower_bound(startKey);
        while (it != map_.end() && it->first <= endKey) {
            keyValues.emplace_back(it->first, it->second);
            ++it;
        }
        return 0;
    }
};
}