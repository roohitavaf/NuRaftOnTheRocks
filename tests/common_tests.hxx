#include "test_common.h"
#include "nuraft.hxx"
#include "common/kv_server_interface.hxx"

using namespace nuraft;
using namespace nuraft_on_the_rocks;

int CRUD_Test(ptr<KeyValueServer> kv_server) {
    // Put
    CHK_EQ(0, kv_server->putKey("Name", "Mo"));
    CHK_EQ(0, kv_server->putKey("Family", "Roohi")); 
    
    // Get
    std::string value;
    CHK_EQ(0, kv_server->getKey("Name", value));
    CHK_EQ(std::string("Mo"), value);

    CHK_EQ(0, kv_server->getKey("Family", value));
    CHK_EQ(std::string("Roohi"), value);

    // Delete
    CHK_EQ(0, kv_server->deleteKey("Family"));
    CHK_NEQ(0, kv_server->getKey("Family", value));

    // Scan
    CHK_EQ(0, kv_server->putKey("Name2", "Mo2"));
    CHK_EQ(0, kv_server->putKey("Name3", "Mo3"));
    CHK_EQ(0, kv_server->putKey("Name4", "Mo4"));

    std::vector<std::pair<std::string, std::string>> keyValues;
    CHK_EQ(0, kv_server->scanKey("Name", "Name3", keyValues));
    std::vector<std::pair<std::string, std::string>> expectedKeyValues = {
        {"Name", "Mo"},
        {"Name2", "Mo2"},
        {"Name3", "Mo3"}
    };
    for (int i = 0; i < 3; i++) {
        CHK_EQ(expectedKeyValues[i].first, keyValues[i].first);
        CHK_EQ(expectedKeyValues[i].second, keyValues[i].second);
    }
    return 0;
}