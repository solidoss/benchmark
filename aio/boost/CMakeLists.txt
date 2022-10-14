add_executable (boost_echo_server boost_echo_server.cpp)

target_link_libraries (boost_echo_server
    ${SYSTEM_BASIC_LIBRARIES}
    uring
)