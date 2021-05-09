# This is the top-level CMakeLists.txt file

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(hash_table CXX)

# C++ standard specification
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC
        include/hash_table.hpp src/hash_table.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)


# executables
add_executable(main main.cpp)
target_link_libraries(main PRIVATE ${PROJECT_NAME})

# dependencies
add_subdirectory(contrib)

# tests
enable_testing()
add_subdirectory(tests)
