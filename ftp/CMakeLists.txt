cmake_minimum_required(VERSION 3.8)
project(ftp)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c client.c server.c)

add_executable(ftp ${SOURCE_FILES} )