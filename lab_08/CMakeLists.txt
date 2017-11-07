cmake_minimum_required(VERSION 3.8)
project(lab_08)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
set(SOURCE_FILES packetCapture.c)

add_executable(lab_08 ${SOURCE_FILES})