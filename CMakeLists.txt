cmake_minimum_required(VERSION 3.10)
project(demo_shared_lib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(demo_shared_lib
        main.cpp
        headers/REXSharedLib.h)
