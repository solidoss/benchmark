add_custom_target(external_build_boost)

if(WIN32)
    #find_path(BOOST_ROOT NAMES boost/asio.hpp PATHS "${EXTERNAL_DIR}/include/boost-1_66")
    file(GLOB BOOST_PATH "${EXTERNAL_DIR}/include/boost-*/boost/asio.hpp")
    get_filename_component(BOOST_ROOT "${BOOST_PATH}" DIRECTORY)
    get_filename_component(BOOST_ROOT "${BOOST_ROOT}" DIRECTORY)
    message("boost path ${BOOST_ROOT}")
    include_directories("${BOOST_ROOT}")
    set(Boost_FOUND TRUE)
else()
    set(BOOST_ROOT "${EXTERNAL_DIR}")
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    #set(Boost_USE_STATIC_RUNTIME ON)

    find_package(Boost
        REQUIRED
        COMPONENTS
            system
            chrono
            thread
            filesystem
            serialization
            program_options
    )
    message("\n-- Boost version: ${Boost_VERSION_STRING}\n")
endif(WIN32)
