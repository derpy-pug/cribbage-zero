cmake_minimum_required(VERSION 3.21.1)
project(Cribbage 
    VERSION 0.1.0 
    LANGUAGES CXX)
set(CRIBBAGE_BINARY_DIR ${CMAKE_BINARY_DIR}/../bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CRIBBAGE_BINARY_DIR})
add_custom_target(run
    COMMAND Cribbage
    DEPENDS Cribbage
    WORKING_DIRECTORY ${CRIBBAGE_BINARY_DIR}
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# include all the files in the src directory recursively
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")
add_executable(Cribbage ${SOURCES})

target_compile_options(Cribbage PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-private-field -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unuse
)
