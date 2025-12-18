# MIT License
# Shared Library CMake Configuration
# Copyright (c) 2025

# Create shared library
add_library(chat_shared STATIC
    common.cpp
)

# Include directories
target_include_directories(chat_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link pthread
find_package(Threads REQUIRED)
target_link_libraries(chat_shared PUBLIC
    Threads::Threads
)

message(STATUS "Configured shared library: chat_shared")
