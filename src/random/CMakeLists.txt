cmake_minimum_required(VERSION 3.20)
project(random 
    VERSION 0.1.0 
    LANGUAGES CXX)

# include all the files in the src directory recursively
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

set(CRIBBAGE_BINARY_DIR ${CMAKE_BINARY_DIR}/../bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CRIBBAGE_BINARY_DIR})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-private-field -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unuse
)

