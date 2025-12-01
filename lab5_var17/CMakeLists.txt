cmake_minimum_required(VERSION 3.14)
project(Lab5StackPMR CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest 
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(pmr_stack
    src/custom_memory_resource.cpp
)

target_include_directories(pmr_stack PUBLIC include)

add_executable(stack_tests tests/test_stack.cpp)
target_link_libraries(stack_tests pmr_stack gtest gtest_main)

add_test(NAME StackAllTests COMMAND stack_tests)
