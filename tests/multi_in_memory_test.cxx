#include "common_tests.hxx"
#include "nuraft.hxx"
#include "multi_in_memory/state_machine.hxx"
#include "common/raft_kv_server.hxx"

#include <string>

using namespace nuraft;
using namespace nuraft_on_the_rocks;

int main(int argc, char** argv) {
    TestSuite ts(argc, argv);
    ts.options.printTestMessage = true;

    ptr<KeyValueStateMachine> sm = cs_new<MultiInMemoryStateMachine>();
    auto kv_server = cs_new<RaftKeyValueServer>(1, 50051, sm);
    ts.doTest("CRUD_Test", CRUD_Test, kv_server);
}