cmake_minimum_required(VERSION 3.21)
project(flippy)

if (ASAN)
    message(STATUS "AddressSanitizer enabled")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

add_subdirectory(3rdparty)
add_subdirectory(src)
