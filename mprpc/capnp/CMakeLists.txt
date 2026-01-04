list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_PATH}")
find_package(CapnProto CONFIG REQUIRED)

message("CapnProto found with version: ${CapnProto_VERSION}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

capnp_generate_cpp(benchSources benchHeaders bench.capnp)

add_executable(capnp_bench_test "bench_test_main.cpp" "bench_client_engine.hpp" "bench_client_engine.cpp" "bench_server_engine.hpp" "bench_server_engine.cpp"
    ${benchSources}
    ${benchHeaders})


target_link_libraries(capnp_bench_test
    CapnProto::capnp-rpc
    Boost::program_options
)


add_test(NAME CapnpPlainNoComp_100_1 COMMAND capnp_bench_test -s 0 --compress 0 -N 100 -t ../test_1.txt)
add_test(NAME CapnpPlainNoComp_100_2 COMMAND capnp_bench_test -s 0 --compress 0 -N 100 -t ../test_2.txt)
add_test(NAME CapnpPlainNoComp_100_3 COMMAND capnp_bench_test -s 0 --compress 0 -N 100 -t ../test_3.txt)
add_test(NAME CapnpPlainNoComp_100_4 COMMAND capnp_bench_test -s 0 --compress 0 -N 100 -t ../test_4.txt)
add_test(NAME CapnpPlainNoComp_100_5 COMMAND capnp_bench_test -s 0 --compress 0 -N 100 -t ../test_5.txt)
add_test(NAME CapnpPlainNoComp_100_6 COMMAND capnp_bench_test -s 0 --compress 0 -N 100 -t ../test_6.txt)

add_test(NAME CapnpPlainNoComp_10_1 COMMAND capnp_bench_test -s 0 --compress 0 -N 10 -l 10000 -t ../test_1.txt)
add_test(NAME CapnpPlainNoComp_10_2 COMMAND capnp_bench_test -s 0 --compress 0 -N 10 -l 10000 -t ../test_2.txt)
add_test(NAME CapnpPlainNoComp_10_3 COMMAND capnp_bench_test -s 0 --compress 0 -N 10 -l 10000 -t ../test_3.txt)
add_test(NAME CapnpPlainNoComp_10_4 COMMAND capnp_bench_test -s 0 --compress 0 -N 10 -l 10000 -t ../test_4.txt)
add_test(NAME CapnpPlainNoComp_10_5 COMMAND capnp_bench_test -s 0 --compress 0 -N 10 -l 10000 -t ../test_5.txt)
add_test(NAME CapnpPlainNoComp_10_6 COMMAND capnp_bench_test -s 0 --compress 0 -N 10 -l 10000 -t ../test_6.txt)
