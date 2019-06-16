cmake_minimum_required(VERSION 3.10)
project(rex2aus)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(rex2aus
        main.cpp
        headers/REXSharedLib.h)
