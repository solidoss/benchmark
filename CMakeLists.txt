cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

#-----------------------------------------------------------------
# The project
#-----------------------------------------------------------------
project (Benchmark)
#-----------------------------------------------------------------

#-----------------------------------------------------------------
# Extra user definitions
#-----------------------------------------------------------------
set(EXTRA_DEFINITIONS "${UDEFS}" CACHE STRING "Extra compiler definitions")
add_definitions(${EXTRA_DEFINITIONS})
#-----------------------------------------------------------------

#-----------------------------------------------------------------
# Prepare the extern path
#-----------------------------------------------------------------

set(EXTERNAL_PATH "${UEXTERNAL}/" CACHE PATH "External prerequisites path")

message("Using prerequisites from: \"${EXTERNAL_PATH}\"")

list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_PATH})

set(SolidFrame_DIR "${EXTERNAL_PATH}/lib/cmake/SolidFrame" CACHE PATH "SolidFrame CMake configuration dir")
find_package(SolidFrame 3)

if(SolidFrame_FOUND)
    message("SolidFrame found with version: ${SolidFrame_VERSION}")
else()
endif()

include(ExternalProject)
include(cmake/build_cxxopts.cmake)

if(EXTERNAL_PATH STREQUAL "")
    set(EXTERNAL_PATH "${CMAKE_CURRENT_BINARY_DIR}/external")
    
    include(cmake/build_boost.cmake)
    include(cmake/build_openssl.cmake)
else()
    #define dummy targets
    add_custom_target(build_boost)
    add_custom_target(build_openssl)
    #add_custom_target(build_snappy)

    include(cmake/find_boost.cmake)
    include(cmake/find_openssl.cmake)
    #include(cmake/find_snappy.cmake)

    message("")
    if(${Boost_FOUND})
        message("Boost found with version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
    else()
        message("Boost not found - some targets will not compile")
    endif()

    if(OPENSSL_FOUND)
        message("OpenSSL found with version: ${OPENSSL_VERSION}")
    else()
        message("OpenSSL not found - some targets will not compile")
    endif()

    message("")
endif()

#-----------------------------------------------------------------
# Prepare the definitions for build types
#-----------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "release")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

if(CMAKE_SYSTEM MATCHES "Linux*")
    message("Detected system ON_LINUX")

    set(SYS_BASIC_LIBS pthread rt)
    set(SYS_DYNAMIC_LOAD_LIB dl)
endif(CMAKE_SYSTEM MATCHES "Linux*")

if(CMAKE_SYSTEM MATCHES "Windows*")
    message("Detected system ON_WINDOWS")
    add_definitions(-D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00)
endif(CMAKE_SYSTEM MATCHES "Windows*")


###############################################################################
# C++ Standard
###############################################################################

set(CMAKE_CXX_STANDARD 17)

#-----------------------------------------------------------------
# Global include directories
#-----------------------------------------------------------------
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${EXTERNAL_PATH}/include/
    ${CMAKE_CURRENT_BINARY_DIR}/external/include
)
#-----------------------------------------------------------------

#-----------------------------------------------------------------
# Global link directories
#-----------------------------------------------------------------
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${EXTERNAL_PATH}/lib
)

#-----------------------------------------------------------------
# Testing support
#-----------------------------------------------------------------

include(CTest)

#-----------------------------------------------------------------
# Project subdirectories
#-----------------------------------------------------------------
add_subdirectory(serialization)
add_subdirectory(mprpc)
add_subdirectory(aio)

include(cmake/source-format.cmake)
