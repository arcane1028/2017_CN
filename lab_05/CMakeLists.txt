cmake_minimum_required(VERSION 3.8)
project(lab_05)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
set(SOURCE_FILES server_thread.c)
add_executable(lab_05 ${SOURCE_FILES})