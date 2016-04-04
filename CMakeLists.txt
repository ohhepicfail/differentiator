cmake_minimum_required(VERSION 3.3)
project(tree)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -std=c++11")

set(SOURCE_FILES main.cpp Node.cpp Node.h Error.cpp Error.h Expression_parser.cpp Expression_parser.h)
add_executable(tree ${SOURCE_FILES})