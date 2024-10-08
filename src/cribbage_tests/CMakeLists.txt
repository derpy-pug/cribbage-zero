cmake_minimum_required(VERSION 3.20)
project(cribbage_tests LANGUAGES CXX)

# Include all .cpp files from the tests directory
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

# Add the test executable
add_executable(${PROJECT_NAME} ${TEST_SOURCES})

add_custom_target(run_tests
    COMMAND ctest --output-on-failure
    DEPENDS cribbage_tests
    COMMENT "Running all tests..."
)

# Link gtest and your core library to the test executable
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main cribbage_core)

# Set the C++ standard to match the project settings
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

# Include the headers of the core library
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cribbage_core/include)

# Enable standard warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-private-field -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-macros
)

# Add a test
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

