#pragma once

#include "nuraft_on_the_rocks.grpc.pb.h"
#include "kv_server_interface.hxx"
#include "libnuraft/nuraft.hxx"

#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include <string>

using namespace nuraft;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

namespace nuraft_on_the_rocks {

class GrpcServer final : public NuRaftOnTheRocks::Service {
private:
    ptr<KeyValueServer> kv_server_;

public:
    GrpcServer(ptr<KeyValueServer> kv_server) : kv_server_(kv_server){}

    Status Put(ServerContext* context, const PutRequest* request, PutResponse* response) override {
        auto key = request->key();
        auto value = request->value();
        if (value.empty()) {
            return grpc::Status(grpc::INVALID_ARGUMENT, "Request failed. Value cannot be empty.");
        }
        int ret = kv_server_->putKey(key, value);
        if (!ret) {
            return Status::OK;
        } else {
            return grpc::Status(grpc::ABORTED, "Request failed. Return code: " + std::to_string(ret));
        }
    }

    Status Get(ServerContext* context, const GetRequest* request, GetResponse* response) override {
        auto key = request->key();
        std::string value;
        int ret = kv_server_->getKey(key, value);
        if (!ret) {
            response->set_value(value);
            return Status::OK;
        }
        return grpc::Status(grpc::ABORTED, "Request failed. Return code: " + std::to_string(ret));
    }

    Status Delete(ServerContext* context, const DeleteRequest* request, DeleteResponse* response) override {
        auto key = request->key();
        int ret = kv_server_->deleteKey(key);
        if (!ret) {
            return Status::OK;
        } else {
            return grpc::Status(grpc::ABORTED, "Request failed. Return code: " + std::to_string(ret));
        }
    }

    Status Scan(ServerContext* context, const ScanRequest* request, ScanResponse* response) override {
        auto startKey = request->start_key();
        auto endKey = request->end_key();
        std::vector<std::pair<std::string, std::string>> keyValues;
        int ret = kv_server_->scanKey(startKey, endKey, keyValues);
        if (!ret) {
            for (auto& pair : keyValues) {
                auto* kv_pair = response->add_results();
                kv_pair->set_key(pair.first);
                kv_pair->set_value(pair.second);
            }
            return Status::OK;
        } else {
            return grpc::Status(grpc::ABORTED, "Request failed. Return code: " + std::to_string(ret));
        }
    }

    grpc::Status AddServer(grpc::ServerContext* context,  const AddServerRequest* request, AddServerResponse* response) override {
        int ret = kv_server_->addServer(request->server_id(), request->server_address());
        if (!ret) {
            return Status::OK;
        } else {
            return grpc::Status(grpc::ABORTED, "Request failed. Return code: " + std::to_string(ret)); 
        }
    }

    grpc::Status ListServers(grpc::ServerContext* context, const ListServersRequest* request, ListServersResponse* response) override {
        std::vector<KeyValueServer::Server> servers;
        int ret = kv_server_->listServers(servers);
        if (!ret) {
            for (auto& server: servers) {
                ServerInfo* info = response->add_servers();
                info->set_server_id(server.id);
                info->set_server_address(server.address);
                info->set_is_leader(server.isLeader);
            }
            return Status::OK;
        } else {
            return grpc::Status(grpc::ABORTED, "Request failed. Return code: " + std::to_string(ret)); 
        }
    }
};
}