add_executable(bench_function bench_function.cpp)
set_property(TARGET bench_function PROPERTY CXX_STANDARD 23)
target_link_libraries(bench_function
    SolidFrame::solid_utility
)

add_test(NAME FunctionSolid1        COMMAND bench_function s 1 10000 10000)
add_test(NAME FunctionStd1          COMMAND bench_function S 1 10000 10000)
add_test(NAME FunctionMStd1         COMMAND bench_function M 1 10000 10000)
add_test(NAME FunctionBoost1        COMMAND bench_function B 1 10000 10000)

add_test(NAME FunctionSolid2        COMMAND bench_function s 2 10000 10000)
add_test(NAME FunctionStd2          COMMAND bench_function S 2 10000 10000)
add_test(NAME FunctionMStd2         COMMAND bench_function M 2 10000 10000)
add_test(NAME FunctionBoost2        COMMAND bench_function B 2 10000 10000)

add_test(NAME FunctionSolid4        COMMAND bench_function s 4 10000 10000)
add_test(NAME FunctionStd4          COMMAND bench_function S 4 10000 10000)
add_test(NAME FunctionMStd4         COMMAND bench_function M 4 10000 10000)
add_test(NAME FunctionBoost4        COMMAND bench_function B 4 10000 10000)


add_test(NAME FunctionSolid8        COMMAND bench_function s 8 10000 10000)
add_test(NAME FunctionStd8          COMMAND bench_function S 8 10000 10000)
add_test(NAME FunctionMStd8         COMMAND bench_function M 8 10000 10000)
add_test(NAME FunctionBoost8        COMMAND bench_function B 8 10000 10000)

add_test(NAME FunctionSolid16       COMMAND bench_function s 16 10000 10000)
add_test(NAME FunctionStd16         COMMAND bench_function S 16 10000 10000)
add_test(NAME FunctionMStd16        COMMAND bench_function M 16 10000 10000)
add_test(NAME FunctionBoost16       COMMAND bench_function B 16 10000 10000)