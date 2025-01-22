add_executable(bench_threadpool_solid bench_threadpool.cpp)

target_link_libraries(bench_threadpool_solid
    SolidFrame::solid_utility
)

add_test(NAME ThreadPoolSolid1 COMMAND bench_threadpool_solid)