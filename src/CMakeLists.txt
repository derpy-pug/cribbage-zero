cmake_minimum_required(VERSION 3.21)
project(Cribbage)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(random)
add_subdirectory(cribbage_core)
add_subdirectory(cli)
add_subdirectory(app)
