cmake_minimum_required(VERSION 3.10)

# Project name
project(OpenGLProject C)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Add source files (use GLOB to gather all .c files in src and glad/src)
file(GLOB SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/glad/src/*.c
)

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    /usr/include   # Adjust this for your environment
    ${CMAKE_SOURCE_DIR}/glad/include
)

# Find and include OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Add executable
add_executable(OpenGLProject ${SOURCES})

# Link libraries
target_link_libraries(OpenGLProject
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    m   # Link math library if needed
)

