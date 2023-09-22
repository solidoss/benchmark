list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_PATH}")
find_package(CapnProto CONFIG REQUIRED)

message("CapnProto found with version: ${CapnProto_VERSION}")

capnp_generate_cpp(benchSources benchHeaders bench.capnp)

add_executable(capnp_bench_test "bench_test_main.cpp" "bench_client_engine.hpp" "bench_client_engine.cpp" "bench_server_engine.hpp" "bench_server_engine.cpp"
    ${benchSources}
    ${benchHeaders})


target_link_libraries(capnp_bench_test
    CapnProto::capnp-rpc
    Boost::program_options
    Boost::system
)