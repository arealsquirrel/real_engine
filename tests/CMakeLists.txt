
project(tests)

file(GLOB_RECURSE TEST_SRCS src/*.cpp)

add_executable(
    tests
    ${TEST_SRCS}
)

target_link_libraries(
    tests
    reallib
    GTest::gtest_main
)

include(GoogleTest) # Required for gtest_discover_tests()
gtest_discover_tests(tests)

