set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(${CMAKE_SOURCE_DIR}/cmake/build_snappy.cmake)


add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/echo-ca-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-server-key.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-server-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-client-key.pem ${CMAKE_CURRENT_BINARY_DIR}/echo-client-cert.pem
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-ca-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-server-key.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-server-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-client-key.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/certs/echo-client-cert.pem ${CMAKE_CURRENT_BINARY_DIR}/${SUFFIX}/
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
    SolidFrame::solid_frame_mprpc
    SolidFrame::solid_frame_aio_openssl
    SolidFrame::solid_serialization_v3
    ${SNAPPY_LIB}
    ${SYS_DYNAMIC_LOAD_LIB}
)
add_dependencies(bench_server_engine bench_certs build_snappy)

add_executable(bench_server bench_server_main.cpp)

target_link_libraries(bench_server
    bench_server_engine
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${SYSTEM_BASIC_LIBRARIES}
)

## Client =====================================================================

add_library(bench_client_engine bench_protocol.hpp bench_client_engine.hpp  bench_client_engine.cpp)

target_link_libraries(bench_client_engine
    SolidFrame::solid_frame_mprpc
    SolidFrame::solid_frame_aio_openssl
    SolidFrame::solid_serialization_v3
    ${SNAPPY_LIB}
    ${SYS_DYNAMIC_LOAD_LIB}
)
add_dependencies(bench_client_engine bench_certs bench_text build_snappy)

add_executable(bench_client bench_client_main.cpp)

target_link_libraries(bench_client
    bench_client_engine
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${SYSTEM_BASIC_LIBRARIES}
)

## Test =====================================================================


add_executable(bench_test bench_test_main.cpp)

add_dependencies(bench_test  bench_text_files)

target_link_libraries(bench_test
    bench_client_engine
    bench_server_engine
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${SYSTEM_BASIC_LIBRARIES}
    ${SYS_DYNAMIC_LOAD_LIB}
)

## Test suite ==============================================================

add_test(NAME SolidPlainNoComp_100_1 COMMAND bench_test -s 0 --compress 0 -N 100 -t ../test_1.txt)
add_test(NAME SolidPlainNoComp_100_2 COMMAND bench_test -s 0 --compress 0 -N 100 -t ../test_2.txt)
add_test(NAME SolidPlainNoComp_100_3 COMMAND bench_test -s 0 --compress 0 -N 100 -t ../test_3.txt)
add_test(NAME SolidPlainNoComp_100_4 COMMAND bench_test -s 0 --compress 0 -N 100 -t ../test_4.txt)
add_test(NAME SolidPlainNoComp_100_5 COMMAND bench_test -s 0 --compress 0 -N 100 -t ../test_5.txt)
add_test(NAME SolidPlainNoComp_100_6 COMMAND bench_test -s 0 --compress 0 -N 100 -t ../test_6.txt)

add_test(NAME SolidPlainComp_100_6 COMMAND bench_test -s 0 --compress 1 -N 100 -t ../test_6.txt)

