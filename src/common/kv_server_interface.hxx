#pragma once
#include <string>

namespace nuraft_on_the_rocks {
class KeyValueServer {
public:
    struct Server {
        std::string id;
        std::string address;
        bool isLeader;
    };

    virtual int getKey(std::string key, std::string& value) = 0;
    virtual int putKey(std::string key, std::string value) = 0;
    virtual int deleteKey(std::string key) = 0;
    virtual int scanKey(std::string startKey, std::string endKey, std::vector<std::pair<std::string, std::string>>& keyValues) = 0;

    int addServer(std::string id, std::string address) {
        return 0;
    }

    int listServers(std::vector<Server>& servers) {
        return 0;
    }
};
}