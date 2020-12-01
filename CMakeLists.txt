cmake_minimum_required(VERSION 3.10)
project(BGC)

link_directories(/usr/lib/llvm-10/lib/)
include_directories(/usr/lib/llvm-10/include/)

add_executable(Main Main.cpp CppParser.cpp)
target_link_libraries(Main -lclang)
