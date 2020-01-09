cmake_minimum_required(VERSION 3.10)
project(BGC)

set(CMAKE_CXX_FLAGS "-lclang -g")
add_executable(Main Main.cpp CppParser.cpp)