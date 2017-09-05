cmake_minimum_required(VERSION 3.8)
project(mp1_clion)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES memtest.c my_allocator.c my_allocator.h)
add_executable(mp1_clion ${SOURCE_FILES})