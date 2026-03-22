cmake_minimum_required(VERSION 3.16)

project(ThreadPoolFuture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(demo
    main.cpp
)

target_include_directories(demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

