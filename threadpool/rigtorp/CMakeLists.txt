set(rt_mpmcq_PREFIX ${CMAKE_BINARY_DIR}/external/rt_mpmcq)

ExternalProject_Add(
    build-rt-mpmcq
    PREFIX ${rt_mpmcq_PREFIX}
    GIT_REPOSITORY "https://github.com/rigtorp/MPMCQueue.git"
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/external/include/rigtorp/"
    BUILD_COMMAND ${CMAKE_COMMAND} -E copy "${rt_mpmcq_PREFIX}/src/build-rt-mpmcq/include/rigtorp/MPMCQueue.h" "${CMAKE_BINARY_DIR}/external/include/rigtorp/"
    INSTALL_COMMAND ""
    #DOWNLOAD_EXTRACT_TIMESTAMP ON
)

include_directories("${CMAKE_BINARY_DIR}/external/include/")

add_compile_options("SHELL: -Wshadow -Wpedantic -mcx16")
add_executable(bench_threadpool_rt bench_threadpool_rt.cpp)

add_dependencies(bench_threadpool_rt  build-rt-mpmcq)

target_link_libraries(bench_threadpool_rt
    SolidFrame::solid_utility
)

add_test(NAME ThreadPoolRigtorp1 COMMAND bench_threadpool_rt)