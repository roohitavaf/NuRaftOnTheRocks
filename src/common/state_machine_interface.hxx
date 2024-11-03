#pragma once 

#include "libnuraft/nuraft.hxx"

#include <string>

using namespace nuraft;

namespace nuraft_on_the_rocks {

class KeyValueStateMachine : public nuraft::state_machine {
public:
    virtual bool getKey(std::string key, std::string& value) = 0;
    virtual bool scan(std::string startKey, std::string endtKey, std::vector<std::pair<std::string, std::string>>& result) = 0;

    // Payload of the Log entries
    struct op_payload {
        std::string key;
        std::string value;
    };

    static ptr<buffer> enc_log(const op_payload& payload) {
        ptr<buffer> ret = buffer::alloc(sizeof(op_payload));
        buffer_serializer bs(ret);

        bs.put_raw(&payload, sizeof(op_payload));

        return ret;
    }
    
    static void dec_log(buffer& log, op_payload& payload_out) {
        assert(log.size() == sizeof(op_payload));
        buffer_serializer bs(log);
        memcpy(&payload_out, bs.get_raw(log.size()), sizeof(op_payload));
    }

    virtual void applyPayload(op_payload payload) = 0;

    ptr<buffer> commit(const ulong log_idx, buffer& data) {
        op_payload payload;
        dec_log(data, payload);

        //Apply the log entry to the state machine
       applyPayload(payload);

        last_committed_idx_ = log_idx;

        // Return Raft log number as a return result.
        ptr<buffer> ret = buffer::alloc( sizeof(log_idx) );
        buffer_serializer bs(ret);
        bs.put_u64(log_idx);
        return ret;
    }

    void commit_config(const ulong log_idx, ptr<cluster_config>& new_conf) {
        // Nothing to do with configuration change. Just update committed index.
        last_committed_idx_ = log_idx;
    }

    bool apply_snapshot(snapshot& s) {
        return true;
    }


    ptr<snapshot> last_snapshot() {
       return nullptr;
    }

    ulong last_commit_index() {
        return last_committed_idx_;
    }

    void create_snapshot(snapshot& s, async_result<bool>::handler_type& when_done) {}

    ptr<buffer> pre_commit(const ulong log_idx, buffer& data) {
        return nullptr;
    }

protected:
    // Last committed Raft log number.
    std::atomic<uint64_t> last_committed_idx_ = 0;

};
}