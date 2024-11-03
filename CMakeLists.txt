cmake_minimum_required(VERSION 3.10)
project(NuRaftOnTheRocks)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -O2")

# Find dependencies
find_package(RocksDB REQUIRED)
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)

# Locate protobuf compiler and gRPC plugin
find_program(PROTOC_EXECUTABLE protoc REQUIRED)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin REQUIRED)

message(STATUS "Protobuf Compiler: ${PROTOC_EXECUTABLE}")
message(STATUS "gRPC Plugin: ${GRPC_CPP_PLUGIN_EXECUTABLE}")

# Set source and output directories
set(SOURCE_FILES src/main.cpp)
set(PROTO_FILE ${CMAKE_SOURCE_DIR}/api/nuraft_on_the_rocks.proto)
set(GENERATED_SRC_DIR ${CMAKE_BINARY_DIR}/generated)

# Ensure the generated directory exists
file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

# Command to generate C++ files from the proto file
add_custom_command(
    OUTPUT ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.pb.cc ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.pb.h
           ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.grpc.pb.cc ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.grpc.pb.h
    COMMAND ${PROTOC_EXECUTABLE}
            --cpp_out=${GENERATED_SRC_DIR}
            --grpc_out=${GENERATED_SRC_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
            --proto_path=${CMAKE_SOURCE_DIR}/api ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating gRPC and Protobuf C++ files from ${PROTO_FILE}"
    VERBATIM
)

# Custom target for generating proto files
add_custom_target(generate_grpc_cpp
    DEPENDS ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.pb.cc ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.pb.h
            ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.grpc.pb.cc ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.grpc.pb.h
    COMMENT "Running generate_grpc_cpp target"
)

# Include directories
include_directories(${GENERATED_SRC_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/NuRaft/include)
if(APPLE)
    message(STATUS "Configuring for macOS")
    # Homebrew paths for macOS
    include_directories("/opt/homebrew/include")
    link_directories("/opt/homebrew/lib")
else()
    message(STATUS "Configuring for Linux or other OS")
    # Add additional include or library paths for Linux if needed
    # You might not need this if libraries are installed in standard locations
    # include_directories("/usr/local/include")  # Example path
    # link_directories("/usr/local/lib")          # Example path
endif()

# Add NuRaft subdirectory
add_subdirectory(libs/NuRaft)


# Define executable
add_executable(${PROJECT_NAME} 
    ${SOURCE_FILES}
    ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.pb.cc
    ${GENERATED_SRC_DIR}/nuraft_on_the_rocks.grpc.pb.cc
    libs/NuRaft/examples/in_memory_log_store.cxx
    libs/NuRaft/examples/logger.cc
)

set(ROCKS_DB_DEPS RocksDB z bz2 snappy lz4 pthread)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    NuRaft::static_lib
    ${ROCKS_DB_DEPS}
    OpenSSL::SSL
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_reflection
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    libs/NuRaft/include/libnuraft
    libs/NuRaft/examples
)

# Make sure the executable depends on the proto generation
add_dependencies(${PROJECT_NAME} generate_grpc_cpp)

# Tests
add_subdirectory(tests)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/runtests.sh ${CMAKE_CURRENT_BINARY_DIR}/runtests.sh COPYONLY)
file(CHMOD ${CMAKE_CURRENT_BINARY_DIR}/runtests.sh PERMISSIONS OWNER_EXECUTE OWNER_READ)

add_custom_target(tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/runtests.sh
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/runtests.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running tests"
)