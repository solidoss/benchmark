set(snappy_PREFIX ${CMAKE_BINARY_DIR}/external/snappy)

if(WIN32)
    set(SNAPPY_LIB ${CMAKE_BINARY_DIR}/external/lib/snappy.lib)
else()
    set(SNAPPY_LIB ${CMAKE_BINARY_DIR}/external/lib/libsnappy.a)
endif()

ExternalProject_Add(
    build_snappy
    EXCLUDE_FROM_ALL 1
    PREFIX ${snappy_PREFIX}
    URL https://github.com/google/snappy/archive/refs/tags/1.2.2.tar.gz
    CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/external -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=release -DSNAPPY_BUILD_TESTS=false -DSNAPPY_BUILD_BENCHMARKS=false
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --config release
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config release --target install
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    BUILD_BYPRODUCTS ${SNAPPY_LIB}
)