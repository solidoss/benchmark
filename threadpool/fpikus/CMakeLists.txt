set(fp_lockfree_PREFIX ${CMAKE_BINARY_DIR}/external/fp_lockfree)

ExternalProject_Add(
    build-fp-lockfree
    PREFIX ${fp_lockfree_PREFIX}
    GIT_REPOSITORY "https://github.com/fpikus/LockFree.git"
    GIT_TAG main
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/external/include/fpikus/"
    BUILD_COMMAND ${CMAKE_COMMAND} -E copy "${fp_lockfree_PREFIX}/src/build-fp-lockfree/ring-atomic-queue.h" "${CMAKE_BINARY_DIR}/external/include/fpikus/"
    INSTALL_COMMAND ""
    #DOWNLOAD_EXTRACT_TIMESTAMP ON
)

include_directories("${CMAKE_BINARY_DIR}/external/include/")

add_compile_options("SHELL: -Wshadow -Wpedantic -mcx16")
add_executable(bench_threadpool_fpikus bench_threadpool_fp.cpp)

add_dependencies(bench_threadpool_fpikus  build-fp-lockfree)

target_link_libraries(bench_threadpool_fpikus
    SolidFrame::solid_utility
)

add_test(NAME ThreadPoolFpikus1 COMMAND bench_threadpool_fpikus)
