
set(gbench_PREFIX ${CMAKE_BINARY_DIR}/external/gbench)

ExternalProject_Add(
    build_gbench
    PREFIX ${gbench_PREFIX}
    URL "https://github.com/google/benchmark/archive/refs/tags/v1.9.4.tar.gz"
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/external
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DBENCHMARK_ENABLE_TESTING=OFF
        -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
        -DBENCHMARK_ENABLE_INSTALL=ON
)

set(GBENCH_LIBS ${CMAKE_BINARY_DIR}/external/lib/libbenchmark.a ${CMAKE_BINARY_DIR}/external/lib/libbenchmark_main.a)

