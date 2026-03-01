cmake_minimum_required(VERSION 3.20)
project(nanovector_project)

# 1. Set the Standard
set(CMAKE_CXX_STANDARD 20)

# CONCEPT: Strict Mode
# This prevents the compiler from silently falling back to C++17 or C++14.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Include Path
# Tells the compiler: "Header files (.h) are living in the 'include' folder"
include_directories(include)

# 3. Source Definitions
set(SOURCE_FILES src/main.cpp)

# 4. Create the App
add_executable(nanovector_project ${SOURCE_FILES})
