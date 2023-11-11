cmake_minimum_required(VERSION 3.20)
project(app 
    VERSION 0.1.0 
    LANGUAGES CXX)

# include all the files in the src directory recursively

add_executable(${PROJECT_NAME} main.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(${PROJECT_NAME} PRIVATE cli)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-private-field -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unuse
)

