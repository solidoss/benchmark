set(cxxopts_PREFIX ${CMAKE_BINARY_DIR}/external/cxxopts)

ExternalProject_Add(
    build-cxxopts
    EXCLUDE_FROM_ALL 1
    PREFIX ${cxxopts_PREFIX}
    URL https://github.com/jarro2783/cxxopts/archive/refs/tags/v3.0.0.tar.gz
    DOWNLOAD_NO_PROGRESS ON
    CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/external -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=release
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --config release
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config release --target install
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
if(SOLID_ON_WINDOWS)
    set(CXXOPTS_LIB ${CMAKE_BINARY_DIR}/external/lib/cxxopts.lib)
else()
    set(CXXOPTS_LIB ${CMAKE_BINARY_DIR}/external/lib/libcxxopts.a)
endif()

