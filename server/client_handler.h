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
 * Handles a single client connection in a separate thread
 * Receives messages and broadcasts them to other clients
 */
class ClientHandler {
public:
    /**
     * Constructor
     * @param socket_fd Client socket file descriptor
     * @param client_id Unique client identifier
     * @param server Pointer to parent server
     */
    ClientHandler(int socket_fd, int client_id, ChatServer* server);

    /**
     * Destructor
     */
    ~ClientHandler();

    /**
     * Main thread function
     * Receives username, then enters message loop
     */
    void run();

private:
    /**
     * Receive and validate username (first message)
     * @return true on success, false on error
     */
    bool receive_username();

    /**
     * Message receiving loop
     * Receives messages and broadcasts them
     */
    void message_loop();

    int socket_fd_;
    int client_id_;
    ChatServer* server_;
    std::string username_;
    std::atomic<bool> should_stop_;
};

#endif // CLIENT_HANDLER_H
