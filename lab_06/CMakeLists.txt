cmake_minimum_required(VERSION 3.8)
project(lab_06)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES prompt_timeout.c)
add_executable(lab_06 ${SOURCE_FILES})