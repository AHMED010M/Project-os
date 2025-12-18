# MIT License
# Multi-threaded Chat System
# Copyright (c) 2025

cmake_minimum_required(VERSION 3.16)
project(os-chat-project VERSION 1.0.0 LANGUAGES CXX)

# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find required packages
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(Threads REQUIRED)

# Enable Qt MOC, RCC, UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/shared)

# Add subdirectories
add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client_gui)
add_subdirectory(tests)

# Print configuration
message(STATUS "==============================================")
message(STATUS "OS Chat Project Configuration")
message(STATUS "==============================================")
message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt5 Version: ${Qt5_VERSION}")
message(STATUS "==============================================")
