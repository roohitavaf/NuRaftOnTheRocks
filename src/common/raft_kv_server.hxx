#pragma once

#include "libnuraft/nuraft.hxx"
#include "in_memory_state_mgr.hxx"
#include "state_machine_interface.hxx"
#include "kv_server_interface.hxx"

#include <string>

using namespace nuraft;

namespace nuraft_on_the_rocks {

class RaftKeyValueServer final : public KeyValueServer {
private:
    ptr<raft_server> server_;
    ptr<raft_launcher> launcher_;
    ptr<KeyValueStateMachine>  my_state_machine_;

public:
    RaftKeyValueServer(int id, int raftPort, ptr<KeyValueStateMachine> stateMachine) {
        ptr<logger>         my_logger = nullptr;
        my_state_machine_ = stateMachine;
        ptr<state_mgr>      my_state_manager = cs_new<inmem_state_mgr>(id, "localhost:" + std::to_string(raftPort));
        asio_service::options   asio_opt;
        raft_params             params;

        launcher_ = cs_new<raft_launcher>();
        server_ = launcher_->init(my_state_machine_, my_state_manager, my_logger, raftPort, asio_opt, params);

        while (!server_->is_initialized()) {
            std::this_thread::sleep_for( std::chrono::milliseconds(100) );
        }
    }

    ~RaftKeyValueServer() {
        launcher_->shutdown();
    }

    int getKey(std::string key, std::string& value) {
        return !my_state_machine_->getKey(key, value);
    }
    int putKey(std::string key, std::string value) {
        ptr<buffer> new_log = KeyValueStateMachine::enc_log({key, value});
        auto result = server_->append_entries({new_log});
        return result->get_result_code();
    }

    int deleteKey(std::string key) {
        ptr<buffer> new_log = KeyValueStateMachine::enc_log({key, ""});
        auto result = server_->append_entries({new_log});
        return result->get_result_code();
    }

    int scanKey(std::string startKey, std::string endKey, std::vector<std::pair<std::string, std::string>>& keyValues) {
        return !my_state_machine_->scan(startKey, endKey, keyValues);
    }

    int addServer(std::string id, std::string address) {
        srv_config srv_conf_to_add(std::stoi(id), address);
        auto result = server_->add_srv(srv_conf_to_add);
        return result->get_result_code();
    }

    int listServers(std::vector<KeyValueServer::Server>& servers) {
        std::vector< ptr<srv_config> > configs;
        server_->get_srv_config_all(configs);

        int leader_id = server_->get_leader();

        for (auto& entry: configs) {
            ptr<srv_config>& srv = entry;
            KeyValueServer::Server server;
            server.id = srv->get_id();
            server.address = srv->get_endpoint();
            server.isLeader = leader_id == srv->get_id();
            servers.push_back(server);
        }
        return 0;
    }
};
}