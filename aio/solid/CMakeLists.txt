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
