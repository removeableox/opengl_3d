# CMakeLists.txt

# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and the languages used
project(opengl_3d C)

# Include directories (add more as needed)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdParty)

# Link directories (add more as needed)
link_directories(${PROJECT_SOURCE_DIR}/thirdParty/GLFW/lib)
link_directories(${PROJECT_SOURCE_DIR}/thirdParty/cglm/lib)

set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
file(COPY ${ASSETS_SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR})
set(SHADERS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
file(COPY ${SHADERS_SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR})

# Specify the C++ standard (optional but recommended)
set(CMAKE_C_STANDARD 11)

# Find and link the required libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Collect all source files in the src directory
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")

# Define the executable and link the libraries
add_executable(main ${SOURCES})
target_link_libraries(main glfw OpenGL::GL)

