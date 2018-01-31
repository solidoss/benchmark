if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_PATH}")


add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/echo-ca-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-server-key.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-server-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-client-key.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-client-cert.pem
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/*.pem ${CMAKE_CURRENT_BINARY_DIR}/
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
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

# gRPC C++ plugin
get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin
    IMPORTED_LOCATION_RELEASE)


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


add_executable(grpc_bench_server "bench_server.cpp"
    ${bench_proto_srcs}
    ${bench_grpc_srcs})

target_link_libraries(grpc_bench_server
    protobuf::libprotobuf
    gRPC::grpc++_unsecure
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY})
    
add_executable(grpc_bench_client "bench_client.cpp"
    ${bench_proto_srcs}
    ${bench_grpc_srcs})
    
add_dependencies(grpc_bench_client grpc_bench_certs grpc_bench_text)

target_link_libraries(grpc_bench_client
    protobuf::libprotobuf
    gRPC::grpc++_unsecure
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY})
