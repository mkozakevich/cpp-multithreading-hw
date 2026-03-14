cmake_minimum_required(VERSION 3.16)

project(ApplyFunction)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

add_executable(apply_function_tests
    apply_function_test.cpp
)

target_link_libraries(apply_function_tests
    GTest::gtest
    GTest::gtest_main
)

add_executable(apply_function_benchmark
    apply_function_benchmark.cpp
)

target_link_libraries(apply_function_benchmark
    benchmark::benchmark
)
