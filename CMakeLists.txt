cmake_minimum_required(VERSION 3.22)
project(ReWinder)

set(CMAKE_CXX_STANDARD 20)

add_executable(ReWinder main.cpp)
target_link_libraries(ReWinder User32)
