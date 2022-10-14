add_executable (echo_client echo_client.cpp)
    
add_dependencies(echo_client build-cxxopts)

target_link_libraries (echo_client
    SolidFrame::solid_frame
    SolidFrame::solid_frame_aio
    SolidFrame::solid_utility
    SolidFrame::solid_system
    ${SYSTEM_BASIC_LIBRARIES}
)

add_subdirectory(solid)
add_subdirectory(boost)