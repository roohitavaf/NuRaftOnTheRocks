#include <iostream> 
#include "run_server.hxx"

void printUsage() {
    std::cout << "Usage: program <node_setup> <storage_mode> <grpc_port_number> [<raft_id>] [<raft_port_number>]\n";
    std::cout << "  <node_setup>            'm' for multi-node or 's' for single-node setup\n";
    std::cout << "  <storage_mode>          'm' for in-memory or 'r' for RocksDB storage\n";
    std::cout << "  <grpc_port_number>      Grpc port number (integer)\n";
    std::cout << "  <raft_id>               Raft ID (integer)\n";
    std::cout << "  <raft_port_number>      Raft port number (integer)\n";
}

bool isInteger(const std::string& str) {
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printUsage();
        return 1;
    }

    std::string nodeSetup = argv[1];
    std::string storageMode = argv[2];
    std::string grpcPortStr = argv[3];

    if ((nodeSetup != "m" && nodeSetup != "s") || (storageMode != "m" && storageMode != "r") || !isInteger(grpcPortStr)) {
        printUsage();
        return 1;
    }
    int grpcPort = std::stoi(grpcPortStr);

    int raftPort = 0;
    int raftId = 0;
    if (nodeSetup == "m") {
        if (argc != 6) {
            printUsage();
            return 1;
        }
        
        std::string raftIdStr = argv[4];
        if (!isInteger(raftIdStr)) {
            printUsage();
            std::cout << "raftId is required when seleting multi-node setup." << std::endl;
            return 1;
        }
        raftId = std::stoi(raftIdStr);

        std::string raftPortStr = argv[5];
        if (!isInteger(raftPortStr)) {
            printUsage();
            std::cout << "raftPort is required when seleting multi-node setup." << std::endl;
            return 1;

        }
        raftPort = std::stoi(raftPortStr);

    }
    
    nuraft_on_the_rocks::RunServer(grpcPort, raftPort, raftId, nodeSetup == "m", storageMode == "r");

    return 0;
}
