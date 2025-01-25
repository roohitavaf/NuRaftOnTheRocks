# NuRaftOnTheRocks

NuRaftOnTheRocks is a replicated key-value store using [RocksDB](https://github.com/facebook/rocksdb) as storage engine and [NuRaft](https://github.com/eBay/NuRaft) for replication.

## How to Build
### Install Dependencies
```
brew install cmake
brew install rocksdb
brew install protobuf
brew install grpc
```

Add NuRaft as a submodule:

```
git submodule add https://github.com/eBay/NuRaft.git libs/NuRaft
git submodule update --init --recursive
```

### Build
```
mkdir build
cd build
cmake ../
make -j8
```

## How to Run
Check the usage.

```
$ ./NuRaftOnTheRocks

Usage: program <node_setup> <storage_mode> <grpc_port_number> [<raft_id>] [<raft_port_number>]
  <node_setup>            'm' for multi-node or 's' for single-node setup
  <storage_mode>          'm' for in-memory or 'r' for RocksDB storage
  <grpc_port_number>      Grpc port number (integer)
  <raft_id>               Raft ID (integer)
  <raft_port_number>      Raft port number (integer)
```

You can combine different types of configuration with different types of storage. For example, to have single-node key-value store with RocksDB storage you can do this:
```
$ ./NuRaftOnTheRocks s r 50052
Server listening on 0.0.0.0:50052
```

Then in another terminal you can interact with the server as follows:
```
$ grpc_cli call localhost:50052 nuraft_on_the_rocks.NuRaftOnTheRocks/Put "key: 'Name' value: 'Mo'"
connecting to localhost:50052
Rpc succeeded with OK status


$ grpc_cli call localhost:50052 nuraft_on_the_rocks.NuRaftOnTheRocks/Get "key: 'Name'"
connecting to localhost:50052
value: "Mo"
Rpc succeeded with OK status
```
### Create a Raft Group

To have a replicated key-value store with RockDB storage we can do as follows: 
Suppose I want two replicas. We first create the first replica

```
$ ./NuRaftOnTheRocks m r 50051 1 10001
Server listening on 0.0.0.0:50051
```
Then I create my second replica in another terminal:
```
$ ./NuRaftOnTheRocks m m 50052 2 10002
Server listening on 0.0.0.0:50052
```

Now, I add the second server to the first server Raft cluster:
```
$ grpc_cli call localhost:50051 nuraft_on_the_rocks.NuRaftOnTheRocks/AddServer "server_id: '2' server_address: 'localhost:10002'"
connecting to localhost:50051
Rpc succeeded with OK status
```

You can list the servers in your Raft group:
```
$ grpc_cli call localhost:50051 nuraft_on_the_rocks.NuRaftOnTheRocks/ListServers ""
connecting to localhost:50051
servers {
  server_id: "\001"
  server_address: "localhost:10001"
  is_leader: true
}
servers {
  server_id: "\002"
  server_address: "localhost:10002"
}
Rpc succeeded with OK status
```

Now, I write to server 1, but I read from server 2 to check the replicaiton is working:
```
$ grpc_cli call localhost:50051 nuraft_on_the_rocks.NuRaftOnTheRocks/Put "key: 'Name' value: 'Mo'"
connecting to localhost:50051
Rpc succeeded with OK status
```

```
$ grpc_cli call localhost:50052 nuraft_on_the_rocks.NuRaftOnTheRocks/Get "key: 'Name'"
connecting to localhost:50052
value: "Mo"
Rpc succeeded with OK status
```



