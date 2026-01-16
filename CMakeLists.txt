cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

#-----------------------------------------------------------------
# Set policies
#-----------------------------------------------------------------

if(${CMAKE_VERSION} VERSION_LESS "3.27.0")
else() 
    cmake_policy(SET CMP0144 NEW)
endif()

if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
	set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0167 NEW)
#-----------------------------------------------------------------

#-----------------------------------------------------------------
# The project
#-----------------------------------------------------------------
project (Benchmark)
#-----------------------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
include(cmake/build_gbench.cmake)

#set(EXTERNAL_PATH "${CMAKE_BINARY_DIR}/external")

if(EXTERNAL_PATH STREQUAL "")
    
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

    find_library(ATOMIC_LIBRARY NAMES atomic atomic.so.1 libatomic.so.1)
    
    set(SYSTEM_BASIC_LIBRARIES pthread rt)
    
    if(ATOMIC_LIBRARY)
        message("Atomic library found: ${ATOMIC_LIBRARY}")
        list(APPEND SYSTEM_BASIC_LIBRARIES atomic)
    endif()

    set(SYS_DYNAMIC_LOAD_LIB dl)
endif(CMAKE_SYSTEM MATCHES "Linux*")

if(CMAKE_SYSTEM MATCHES "Windows*")
    message("Detected system ON_WINDOWS")
    add_definitions(-D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00)
endif(CMAKE_SYSTEM MATCHES "Windows*")


###############################################################################
# C++ Standard
###############################################################################

set(CMAKE_CXX_STANDARD 20)

#-----------------------------------------------------------------
# Global include directories
#-----------------------------------------------------------------
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    "${CMAKE_BINARY_DIR}/external/include"
    "${EXTERNAL_PATH}/include"
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
add_subdirectory(aio)
add_subdirectory(serialization)
add_subdirectory(mprpc)
add_subdirectory(threadpool)
add_subdirectory(function)

include(cmake/source-format.cmake)
