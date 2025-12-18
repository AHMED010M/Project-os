# MIT License
# Client CMake Configuration
# Copyright (c) 2025

# Create client executable
add_executable(chat_client
    main.cpp
    MainWindow.cpp
    SocketClient.cpp
    ShmClient.cpp
)

# Include directories
target_include_directories(chat_client PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
)

# Link libraries
target_link_libraries(chat_client PRIVATE
    chat_shared
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Threads::Threads
)

message(STATUS "Configured client executable: chat_client")
