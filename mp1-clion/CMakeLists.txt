cmake_minimum_required(VERSION 3.8)
project(mp1_clion)

set(CMAKE_C_STANDARD 11)
SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -pedantic -O0")
set(SOURCE_FILES memtest.c my_allocator.c my_allocator.h free_list.h free_list.c utils.h utils.c)
add_executable(mp1_clion ${SOURCE_FILES})

target_link_libraries(mp1_clion m)