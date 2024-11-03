#include "../common/kv_server_interface.hxx"

#include <string>
#include <map>
#include <mutex>

namespace nuraft_on_the_rocks {

class SingleInMemory final : public KeyValueServer {
private:
    std::map<std::string, std::string> map_;
    std::mutex mtx_;

public:
    int getKey(std::string key, std::string& value) {
        std::unique_lock<std::mutex> lk(mtx_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            value = it->second;
            return 0;
        } else {
            return -1;
        }
    }
    int putKey(std::string key, std::string value) {
        std::unique_lock<std::mutex> lk(mtx_);
        map_[key] = value;
        return 0;
    }
    int deleteKey(std::string key) {
        std::unique_lock<std::mutex> lk (mtx_);
        map_.erase(key);
        return 0;

    }
    int scanKey(std::string startKey, std::string endKey, std::vector<std::pair<std::string, std::string>>& keyValues) {
        std::unique_lock<std::mutex> lk(mtx_);
        auto it = map_.lower_bound(startKey);
        while (it != map_.end() && it->first <= endKey) {
            keyValues.emplace_back(it->first, it->second);
            ++it;
        }
        return 0;
    }
};
}