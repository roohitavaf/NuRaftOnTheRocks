
#include "single_in_memory/kv_server_impl.hxx"
#include "single_rocksdb/kv_server_impl.hxx"
#include "multi_in_memory/state_machine.hxx"
#include "common/raft_kv_server.hxx"
#include "common/grpc_server.hxx"
#include "libnuraft/nuraft.hxx"

namespace nuraft_on_the_rocks {
std::unique_ptr<grpc::Server> server;

void signalHandler(int signum) {
    if (server) {
        server->Shutdown();
        std::cout << "Server shutting down." << std::endl;
    }
}

void RunServer(int grpcPort, int raftPort, int raftId, bool multi, bool rocksdb) {
    std::string server_address("0.0.0.0:" + std::to_string(grpcPort));

    ptr<KeyValueServer> kv_server;
    if (multi) {
        // For the multi-node case, we need a Raft state machine
        ptr<KeyValueStateMachine> sm;
        if (rocksdb) {
            exit(1); //Not implemented
        } else {
            // State machine backed by an in-memory map
            sm = cs_new<MultiInMemoryStateMachine>();
        }
        kv_server = cs_new<RaftKeyValueServer>(raftId, raftPort, sm);
    } else {
        if (rocksdb) {
            kv_server = cs_new<SingleRocksDB>();

        } else {
            kv_server = cs_new<SingleInMemory>();
        }
    }
    NuRaftOnTheRocks::Service* service =  new GrpcServer(kv_server);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(service);

    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    server = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    server->Wait();
    delete service;
}
}

