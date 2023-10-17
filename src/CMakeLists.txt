cmake_minimum_required(VERSION 3.21.1)
project(Cribbage 
    VERSION 0.1.0 
    LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# include all the files in the src directory recursively
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")
#file(GLOB SOURCES "src/*.cpp")
#add_executable(Cribbage main.cpp board.cpp)
add_executable(Cribbage ${SOURCES})
#target_sources(Cribbage PRIVATE ${SOURCES})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#install(TARGETS Cribbage DESTINATION bin)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-private-field -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unuse")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

