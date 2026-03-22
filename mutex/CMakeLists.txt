cmake_minimum_required(VERSION 3.10)

project(mutex)

set(CMAKE_CXX_STANDARD 17)

add_executable(mutex main.cpp)

target_compile_options(mutex PRIVATE -O2 -pthread)
target_link_libraries(mutex pthread)
