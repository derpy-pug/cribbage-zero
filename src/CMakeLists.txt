cmake_minimum_required(VERSION 3.21)
project(Cribbage)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

enable_testing()

# Add the CMake modules to the project
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz 
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Download the content
FetchContent_MakeAvailable(googletest)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(random)
add_subdirectory(cribbage_core)
add_subdirectory(cli)
add_subdirectory(app)
add_subdirectory(cribbage_tests)
