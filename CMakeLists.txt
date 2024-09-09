cmake_minimum_required(VERSION 3.14)
project(fdapdepy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WARNINGS_FLAGS "-Wall -Wpedantic -Wextra -Werror")
set(CMAKE_CXX_FLAGS "-g -O2 -march=x86-64 -std=c++20 ${CMAKE_WARNINGS_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}/include/fdaPDE-cpp/fdaPDE/core/")

find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_subdirectory(pybind11)
include_directories(include/fdaPDE-cpp/fdaPDE/)

pybind11_add_module(fdapdepy src/main.cpp)
target_link_libraries (fdapdepy PUBLIC Eigen3::Eigen)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(fdapdepy
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

