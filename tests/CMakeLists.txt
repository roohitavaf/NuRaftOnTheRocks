add_executable(single_in_memory_test
                single_in_memory_test.cxx)

target_include_directories(single_in_memory_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/tests
    ${CMAKE_SOURCE_DIR}/libs/NuRaft/include/libnuraft
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(single_in_memory_test
    PRIVATE
    NuRaft::static_lib
)
