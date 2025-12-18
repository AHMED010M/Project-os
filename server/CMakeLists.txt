# MIT License
# Server CMake Configuration
# Copyright (c) 2025

# Create server executable
add_executable(chat_server
    main.cpp
    server.cpp
    client_handler.cpp
)

# Include shared directory
target_include_directories(chat_server PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
)

# Link libraries
target_link_libraries(chat_server PRIVATE
    chat_shared
    Threads::Threads
)

message(STATUS "Configured server executable: chat_server")
