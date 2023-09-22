add_subdirectory(solid)
add_subdirectory(grpc)
add_subdirectory(capnp)

add_executable(text_generator text_generator.cpp)

target_link_libraries(text_generator
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/test_1.txt
        ${CMAKE_CURRENT_BINARY_DIR}/test_2.txt
        ${CMAKE_CURRENT_BINARY_DIR}/test_3.txt
        ${CMAKE_CURRENT_BINARY_DIR}/test_4.txt
        ${CMAKE_CURRENT_BINARY_DIR}/test_5.txt
        ${CMAKE_CURRENT_BINARY_DIR}/test_6.txt
    COMMAND text_generator -w 10 -W 100 -l 100 -L 1000 -c 10 -C 100 -f test_1.txt 
    COMMAND text_generator -w 10 -W 100 -l 100 -L 10000 -c 10 -C 100 -f test_2.txt
    COMMAND text_generator -w 10 -W 1000 -l 100 -L 10000 -c 10 -C 100 -f test_3.txt
    COMMAND text_generator -w 10 -W 10000 -l 100 -L 60000 -c 10 -C 100 -f test_4.txt
    COMMAND text_generator -w 10 -W 10000 -l 1000 -L 100000 -c 10 -C 100 -f test_5.txt
    COMMAND text_generator -w 10 -W 100 -l 1000 -L 100000 -c 10 -C 100 -f test_6.txt
    DEPENDS text_generator
)

add_custom_target(bench_text_files
    DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/test_1.txt
    ${CMAKE_CURRENT_BINARY_DIR}/test_2.txt
    ${CMAKE_CURRENT_BINARY_DIR}/test_3.txt
    ${CMAKE_CURRENT_BINARY_DIR}/test_4.txt
    ${CMAKE_CURRENT_BINARY_DIR}/test_5.txt
    ${CMAKE_CURRENT_BINARY_DIR}/test_6.txt
)
