cmake_minimum_required(VERSION 3.0)
project(mini_ringbuffer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(mini_ringbuffer ${CMAKE_SOURCE_DIR}/src/ringbuffer.cpp test.cpp)