cmake_minimum_required(VERSION 3.10)

project(SimpleCProject C)

set(SOURCES main.c multiply.c)

add_executable(myprogram ${SOURCES})
