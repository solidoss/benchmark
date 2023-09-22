add_executable (solid_relay_server solid_relay_server.cpp)
    
add_dependencies(solid_relay_server build-cxxopts)

target_link_libraries (solid_relay_server
    SolidFrame::solid_frame
    SolidFrame::solid_frame_aio
    SolidFrame::solid_utility
    SolidFrame::solid_system
    ${SYSTEM_BASIC_LIBRARIES}
)

add_executable (solid_echo_server solid_echo_server.cpp)
    
add_dependencies(solid_echo_server build-cxxopts)

target_link_libraries (solid_echo_server
    SolidFrame::solid_frame
    SolidFrame::solid_frame_aio
    SolidFrame::solid_utility
    SolidFrame::solid_system
    ${SYSTEM_BASIC_LIBRARIES}
)

#add_test(NAME SolidEcho100 COMMAND bench_test -s 0 --compress 0 -N 100 -t ../test_1.txt)
add_test(SolidEcho100_1000 sh -c "(trap 'kill 0' SIGINT SIGTERM; ./solid_echo_server -l 12345 --count 100 & ../echo_client -N 100 -c localhost:12345 -R 1000; wait)")
add_test(SolidEcho200_1000 sh -c "(trap 'kill 0' SIGINT SIGTERM; ./solid_echo_server -l 12345 --count 200 & ../echo_client -N 200 -c localhost:12345 -R 1000; wait)")
add_test(SolidEcho400_1000 sh -c "(trap 'kill 0' SIGINT SIGTERM; ./solid_echo_server -l 12345 --count 400 & ../echo_client -N 400 -c localhost:12345 -R 1000; wait)")

add_test(SolidRelayEcho100_1000 sh -c "(trap 'kill 0' SIGINT SIGTERM; ./solid_echo_server -l 12345 --count 100 & ./solid_relay_server -l 20000 -c localhost:12345 --count 100 & ../echo_client -N 100 -c localhost:20000 -R 1000; wait)")
add_test(SolidRelayEcho200_1000 sh -c "(trap 'kill 0' SIGINT SIGTERM; ./solid_echo_server -l 12345 --count 200 & ./solid_relay_server -l 20000 -c localhost:12345 --count 200 & ../echo_client -N 200 -c localhost:20000 -R 1000; wait)")
add_test(SolidRelayEcho400_1000 sh -c "(trap 'kill 0' SIGINT SIGTERM; ./solid_echo_server -l 12345 --count 400 & ./solid_relay_server -l 20000 -c localhost:12345 --count 400 & ../echo_client -N 400 -c localhost:20000 -R 1000; wait)")