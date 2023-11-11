cmake_minimum_required(VERSION 3.21)
project(Cribbage)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(random)
add_subdirectory(cribbage_core)
add_subdirectory(cli)
add_subdirectory(app)
