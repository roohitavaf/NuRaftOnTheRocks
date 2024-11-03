#include "common.hxx"
#include "nuraft.hxx"
#include "single_in_memory/kv_server_impl.hxx"

#include <string>

using namespace nuraft;
using namespace nuraft_on_the_rocks;

int main(int argc, char** argv) {
    TestSuite ts(argc, argv);
    ts.options.printTestMessage = true;

    auto kv_server = cs_new<SingleInMemory>();
    ts.doTest("CRUD_Test", CRUD_Test, kv_server);
}