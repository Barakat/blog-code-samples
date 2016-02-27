cmake_minimum_required(VERSION 3.3)
project(untitled1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(untitled1 ${SOURCE_FILES})

target_link_libraries(untitled1 glew32 glfw3 opengl32)

