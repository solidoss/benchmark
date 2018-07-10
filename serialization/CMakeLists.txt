include(ExternalProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_SOURCE_DIR}/cmake/build_cereal.cmake)

add_executable(benchmark main.cpp boost/record.cpp solid_v1/record.cpp cereal/record.cpp solid_v2/record.cpp)

add_dependencies(benchmark build_cereal)

target_link_libraries(benchmark
    solid_serialization_v1
    solid_serialization_v2
    solid_utility
    solid_system
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${SYS_BASIC_LIBS}
)

add_test(NAME Boost1k COMMAND benchmark 1000 boost)
add_test(NAME Cereal1k COMMAND benchmark 1000 cereal)
add_test(NAME CerealPortable1k COMMAND benchmark 1000 cerealp)
add_test(NAME SolidV1_1k COMMAND benchmark 1000 solid_v1)
add_test(NAME SolidV2_1k COMMAND benchmark 1000 solid_v2)

add_test(NAME Boost10k COMMAND benchmark 10000 boost)
add_test(NAME Cereal10k COMMAND benchmark 10000 cereal)
add_test(NAME CerealPortable10k COMMAND benchmark 10000 cerealp)
add_test(NAME SolidV1_10k COMMAND benchmark 10000 solid_v1)
add_test(NAME SolidV2_10k COMMAND benchmark 10000 solid_v2)

add_test(NAME Boost100k COMMAND benchmark 100000 boost)
add_test(NAME Cereal100k COMMAND benchmark 100000 cereal)
add_test(NAME CerealPortable100k COMMAND benchmark 100000 cerealp)
add_test(NAME SolidV1_100k COMMAND benchmark 100000 solid_v1)
add_test(NAME SolidV2_100k COMMAND benchmark 100000 solid_v2)

