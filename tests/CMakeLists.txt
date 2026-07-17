
project(tests)
file(GLOB_RECURSE TEST_SRCS src/*.cpp)
add_executable(tests ${TEST_SRCS})
target_link_libraries(
    tests
    reallib
    GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(tests)

project(benchmarks)
file(GLOB_RECURSE BENCH_SRCS benchmarks/*.cpp)
add_executable(benchmarks ${BENCH_SRCS})
target_link_libraries(
    benchmarks
    reallib
)

