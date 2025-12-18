// MIT License
// Multi-threaded Chat System - Client Handler Header
// Copyright (c) 2025

#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "protocol.h"
#include <atomic>

// Forward declaration
class ChatServer;

/**
 * Handles a single client connection
 * Runs in its own thread
 */
class ClientHandler {
public:
    /**
     * Constructor
     * @param socket_fd Client socket file descriptor
     * @param client_id Unique client identifier
     * @param server Pointer to server instance
     */
    ClientHandler(int socket_fd, int client_id, ChatServer* server);

    /**
     * Destructor - ensures cleanup
     */
    ~ClientHandler();

    /**
     * Main entry point for client handler thread
     * Receives username, then enters message loop
     */
    void run();

private:
    int socket_fd_;                     // Client socket
    int client_id_;                     // Client ID
    ChatServer* server_;                // Server instance
    std::atomic<bool> should_stop_;     // Stop flag
    std::string username_;              // Client username

    /**
     * Receive and validate username
     * First message from client must be username
     * @return true if username received successfully
     */
    bool receive_username();

    /**
     * Main message loop
     * Receives messages and broadcasts them
     */
    void message_loop();

    /**
     * Cleanup resources
     */
    void cleanup();
};

#endif // CLIENT_HANDLER_H
