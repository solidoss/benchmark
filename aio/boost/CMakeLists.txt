add_executable (boost_echo_server boost_echo_server.cpp)

target_link_libraries (boost_echo_server
    ${SYSTEM_BASIC_LIBRARIES}
    #uring
)

add_executable (boost_relay_server boost_relay_server.cpp)

target_link_libraries (boost_relay_server
    ${SYSTEM_BASIC_LIBRARIES}
    
    #uring
)

add_test(BoostEcho100_1000 sh -c "(trap 'kill 0' SIGINT; ./boost_echo_server 2000 100 & ../echo_client -N 100 -c localhost:2000 -R 1000)")
add_test(BoostEcho200_1000 sh -c "(trap 'kill 0' SIGINT; ./boost_echo_server 2000 200 & ../echo_client -N 200 -c localhost:2000 -R 1000)")
add_test(BoostEcho400_1000 sh -c "(trap 'kill 0' SIGINT; ./boost_echo_server 2000 400 & ../echo_client -N 400 -c localhost:2000 -R 1000)")


add_test(BoostRelayEcho100_1000 sh -c "(trap 'kill 0' SIGINT; ./boost_echo_server 2000 100 & ./boost_relay_server 3000 2000 100 & ../echo_client -N 100 -c localhost:3000 -R 1000)")
add_test(BoostRelayEcho200_1000 sh -c "(trap 'kill 0' SIGINT; ./boost_echo_server 2000 200 & ./boost_relay_server 3000 2000 200 & ../echo_client -N 200 -c localhost:3000 -R 1000)")
add_test(BoostRelayEcho400_1000 sh -c "(trap 'kill 0' SIGINT; ./boost_echo_server 2000 400 & ./boost_relay_server 3000 2000 400 & ../echo_client -N 400 -c localhost:3000 -R 1000)")
