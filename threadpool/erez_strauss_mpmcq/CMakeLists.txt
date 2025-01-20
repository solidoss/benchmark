set(es_mpmcq_PREFIX ${CMAKE_BINARY_DIR}/external/cereal)

ExternalProject_Add(
    build-es-mpmcq
    PREFIX ${es_mpmcq_PREFIX}
    GIT_REPOSITORY "https://github.com/erez-strauss/lockfree_mpmc_queue.git"
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/external/include/es/lockfree/"
    BUILD_COMMAND ${CMAKE_COMMAND} -E copy "${es_mpmcq_PREFIX}/src/build-es-mpmcq/pointer_mpmc_queue.h" "${CMAKE_BINARY_DIR}/external/include/es/lockfree/"
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy "${es_mpmcq_PREFIX}/src/build-es-mpmcq/mpmc_queue.h" "${CMAKE_BINARY_DIR}/external/include/es/lockfree/"
    #DOWNLOAD_EXTRACT_TIMESTAMP ON
)

include_directories("${CMAKE_BINARY_DIR}/external/include/es/lockfree/")
#set( CMAKE_CXX_FLAGS "-DCOMPARE_BOOST=0 -std=c++17 -W -Wall -Wshadow -Wpedantic -ggdb3 -O3 -mcx16 " )

add_compile_options("SHELL: -Wshadow -Wpedantic -mcx16")
add_executable(bench_threadpool_es bench_threadpool_es.cpp)

add_dependencies(bench_threadpool_es  build-es-mpmcq)

target_link_libraries(bench_threadpool_es
    SolidFrame::solid_utility
)

