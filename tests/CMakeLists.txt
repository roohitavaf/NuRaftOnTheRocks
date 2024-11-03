# Single In-Memory
add_executable(single_in_memory_test
                single_in_memory_test.cxx)

target_include_directories(single_in_memory_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/tests
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/include/libnuraft
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(single_in_memory_test
    PRIVATE
    NuRaft::static_lib
)

# Single RocksDB
add_executable(single_rocksdb_test
                single_rocksdb_test.cxx)

target_include_directories(single_rocksdb_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/tests
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/include/libnuraft
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(single_rocksdb_test
    PRIVATE
    NuRaft::static_lib
    ${ROCKS_DB_DEPS}
)

# Multi In-Memory
add_executable(multi_in_memory_test
                multi_in_memory_test.cxx
                ${CMAKE_SOURCE_DIR}/libs/NuRaft/examples/in_memory_log_store.cxx
                ${CMAKE_SOURCE_DIR}/libs/NuRaft/examples/logger.cc
)

target_include_directories(multi_in_memory_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/tests
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/include/libnuraft
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/examples
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(multi_in_memory_test
    PRIVATE
    NuRaft::static_lib
    OpenSSL::SSL
)

# Multi RocksDB
add_executable(multi_rocksdb_test
                multi_rocksdb_test.cxx
                ${CMAKE_SOURCE_DIR}/libs/NuRaft/examples/in_memory_log_store.cxx
                ${CMAKE_SOURCE_DIR}/libs/NuRaft/examples/logger.cc
)

target_include_directories(multi_rocksdb_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/tests
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/include/libnuraft
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/examples
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(multi_rocksdb_test
    PRIVATE
    NuRaft::static_lib
    ${ROCKS_DB_DEPS}
    OpenSSL::SSL
)

configure_file(${CMAKE_SOURCE_DIR}/scripts/runtests.sh ${CMAKE_CURRENT_BINARY_DIR}/runtests.sh COPYONLY)
file(CHMOD ${CMAKE_CURRENT_BINARY_DIR}/runtests.sh PERMISSIONS OWNER_EXECUTE OWNER_READ)


set(TEST_EXECUTABLES
    single_in_memory_test
    single_rocksdb_test
    multi_in_memory_test
    multi_rocksdb_test
)

add_custom_target(tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/runtests.sh
    DEPENDS ${TEST_EXECUTABLES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running tests"
)