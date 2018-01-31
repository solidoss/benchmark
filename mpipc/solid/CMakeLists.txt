set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(${CMAKE_SOURCE_DIR}/cmake/build_snappy.cmake)


add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/echo-ca-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-server-key.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-server-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-client-key.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-client-cert.pem
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/*.pem ${CMAKE_CURRENT_BINARY_DIR}/
    DEPENDS ${PROJECT_SOURCE_DIR}/certs/echo-ca-cert.pem ${PROJECT_SOURCE_DIR}/certs/echo-server-key.pem ${PROJECT_SOURCE_DIR}/certs/echo-server-cert.pem ${PROJECT_SOURCE_DIR}/certs/echo-client-key.pem ${PROJECT_SOURCE_DIR}/certs/echo-client-cert.pem
)

add_custom_target(bench_certs
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

add_custom_target(bench_text
    DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/test_text.txt
)

## Server =====================================================================

add_library(bench_server_engine bench_protocol.hpp bench_server_engine.hpp  bench_server_engine.cpp)

target_link_libraries(bench_server_engine
    solid_frame_mpipc
    solid_frame_aio_openssl
    solid_serialization
    ${SNAPPY_LIB}
)
add_dependencies(bench_server_engine bench_certs build_snappy)

add_executable(bench_server bench_server_main.cpp)

target_link_libraries(bench_server
    bench_server_engine
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${SYS_BASIC_LIBS}
    ${SYS_DYNAMIC_LOAD_LIB}
)

## Client =====================================================================

add_library(bench_client_engine bench_protocol.hpp bench_client_engine.hpp  bench_client_engine.cpp)

target_link_libraries(bench_client_engine
    solid_frame_mpipc
    solid_frame_aio_openssl
    solid_serialization
    ${SNAPPY_LIB}
)
add_dependencies(bench_client_engine bench_certs bench_text build_snappy)

add_executable(bench_client bench_client_main.cpp)

target_link_libraries(bench_client
    bench_client_engine
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${SYS_BASIC_LIBS}
    ${SYS_DYNAMIC_LOAD_LIB}
)
