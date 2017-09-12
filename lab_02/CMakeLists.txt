cmake_minimum_required(VERSION 3.8)
project(lab_02)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c udpclient.c)
add_executable(lab_02 ${SOURCE_FILES})