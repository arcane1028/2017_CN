cmake_minimum_required(VERSION 3.8)
project(lab_03)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        client.c
        server.c)

add_executable(lab_03 ${SOURCE_FILES})