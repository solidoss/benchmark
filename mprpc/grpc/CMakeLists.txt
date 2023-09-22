
list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_PATH}")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/echo-ca-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-server-key.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-server-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-client-key.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-client-cert.pem
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-ca-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-server-key.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-server-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-client-key.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-client-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    DEPENDS ${PROJECT_SOURCE_DIR}/certs/echo-ca-cert.pem ${PROJECT_SOURCE_DIR}/certs/echo-server-key.pem ${PROJECT_SOURCE_DIR}/certs/echo-server-cert.pem ${PROJECT_SOURCE_DIR}/certs/echo-client-key.pem ${PROJECT_SOURCE_DIR}/certs/echo-client-cert.pem
)


add_custom_target(grpc_bench_certs
    DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/echo-ca-cert.pem
    ${CMAKE_CURRENT_BINARY_DIR}/echo-server-key.pem
    ${CMAKE_CURRENT_BINARY_DIR}/echo-server-cert.pem
    ${CMAKE_CURRENT_BINARY_DIR}/echo-client-key.pem
    ${CMAKE_CURRENT_BINARY_DIR}/echo-client-cert.pem
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test_text.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../test_text.txt ${CMAKE_CURRENT_BINARY_DIR}/
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../test_text.txt
)

add_custom_target(grpc_bench_text
    DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/test_text.txt
)

# Protobuf
set(protobuf_MODULE_COMPATIBLE ON CACHE BOOL "")
#set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf REQUIRED)
find_package(utf8_range REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Using protobuf ${protobuf_VERSION}")

# gRPC
find_package(gRPC REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

# gRPC C++ plugin
set(gRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Proto file
get_filename_component(bench_proto "bench.proto" ABSOLUTE)
get_filename_component(bench_proto_path "${bench_proto}" PATH)

# Generated sources
protobuf_generate_cpp(bench_proto_srcs bench_proto_hdrs "${bench_proto}")
set(bench_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/bench.grpc.pb.cc")
set(bench_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bench.grpc.pb.h")

add_custom_command(
      OUTPUT "${bench_grpc_srcs}" "${bench_grpc_hdrs}"
      COMMAND protobuf::protoc
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}" -I "${bench_proto_path}"
        --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}"
        "${bench_proto}"
      DEPENDS "${bench_proto}")

# Generated include directory
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


add_executable(grpc_bench_server "bench_server_main.cpp" "bench_server_engine.hpp" "bench_server_engine.cpp"
    ${bench_proto_srcs}
    ${bench_grpc_srcs})

target_link_libraries(grpc_bench_server
    absl::flags
    absl::flags_parse
    absl::absl_check
    absl::absl_log
    protobuf::libprotobuf
    gRPC::grpc++_unsecure
    Boost::program_options
    Boost::system
    utf8_range::utf8_range
    utf8_range::utf8_validity
)
    
add_executable(grpc_bench_client "bench_client_main.cpp" "bench_client_engine.hpp" "bench_client_engine.cpp"
    ${bench_proto_srcs}
    ${bench_grpc_srcs})
    
add_dependencies(grpc_bench_client grpc_bench_certs grpc_bench_text)

target_link_libraries(grpc_bench_client
    absl::flags
    absl::flags_parse
    absl::absl_check
    absl::absl_log
    protobuf::libprotobuf
    gRPC::grpc++_unsecure
    Boost::program_options
    Boost::system
    utf8_range::utf8_range
    utf8_range::utf8_validity
)    
add_executable(grpc_bench_test "bench_test_main.cpp" "bench_client_engine.hpp" "bench_client_engine.cpp" "bench_server_engine.hpp" "bench_server_engine.cpp"
    ${bench_proto_srcs}
    ${bench_grpc_srcs})
    
add_dependencies(grpc_bench_test grpc_bench_certs bench_text_files)

target_link_libraries(grpc_bench_test
    absl::flags
    absl::flags_parse
    absl::absl_check
    absl::absl_log
    protobuf::libprotobuf
    gRPC::grpc++_unsecure
    Boost::program_options
    Boost::system
    utf8_range::utf8_range
    utf8_range::utf8_validity
)

## Test suite ==============================================================

add_test(NAME GrpcPlainNoComp_100_1 COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -t ../test_1.txt)
add_test(NAME GrpcPlainNoComp_100_2 COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -t ../test_2.txt)
add_test(NAME GrpcPlainNoComp_100_3 COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -t ../test_3.txt)
add_test(NAME GrpcPlainNoComp_100_4 COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -t ../test_4.txt)
add_test(NAME GrpcPlainNoComp_100_5 COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -t ../test_5.txt)
add_test(NAME GrpcPlainNoComp_100_6 COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -t ../test_6.txt)

add_test(NAME GrpcPlainNoComp_100_1_Stream COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -S -t ../test_1.txt)
add_test(NAME GrpcPlainNoComp_100_2_Stream COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -S -t ../test_2.txt)
add_test(NAME GrpcPlainNoComp_100_3_Stream COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -S -t ../test_3.txt)
add_test(NAME GrpcPlainNoComp_100_4_Stream COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -S -t ../test_4.txt)
add_test(NAME GrpcPlainNoComp_100_5_Stream COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -S -t ../test_5.txt)
add_test(NAME GrpcPlainNoComp_100_6_Stream COMMAND grpc_bench_test -s 0 --compress 0 -N 100 -S -t ../test_6.txt)
