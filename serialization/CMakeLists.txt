include(ExternalProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_SOURCE_DIR}/cmake/build_cereal.cmake)

add_executable(benchmark main.cpp boost/record.cpp cereal/record.cpp solid_v3/record.cpp)

add_dependencies(benchmark build_cereal)

target_link_libraries(benchmark
    SolidFrame::solid_serialization_v3
    SolidFrame::solid_utility
    SolidFrame::solid_system
    Boost::serialization
    Boost::program_options
    Boost::system
    ${SYS_BASIC_LIBS}
)

add_test(NAME Boost1k COMMAND benchmark 1000 boost)
add_test(NAME Cereal1k COMMAND benchmark 1000 cereal)
add_test(NAME CerealPortable1k COMMAND benchmark 1000 cerealp)
add_test(NAME SolidV3_1k COMMAND benchmark 1000 solid)

add_test(NAME Boost10k COMMAND benchmark 10000 boost)
add_test(NAME Cereal10k COMMAND benchmark 10000 cereal)
add_test(NAME CerealPortable10k COMMAND benchmark 10000 cerealp)
add_test(NAME SolidV3_10k COMMAND benchmark 10000 solid)

add_test(NAME Boost100k COMMAND benchmark 100000 boost)
add_test(NAME Cereal100k COMMAND benchmark 100000 cereal)
add_test(NAME CerealPortable100k COMMAND benchmark 100000 cerealp)
add_test(NAME SolidV3_100k COMMAND benchmark 100000 solid)

