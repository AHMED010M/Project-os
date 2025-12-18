// MIT License
// Multi-threaded Chat System - Server Header
// Copyright (c) 2025

#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

/**
 * Multi-threaded TCP chat server
 * Handles multiple concurrent client connections
 * Broadcasts messages to all clients except sender
 */
class ChatServer {
public:
    /**
     * Constructor
     * @param host IP address to bind to (0.0.0.0 for all interfaces)
     * @param port Port number to listen on
     */
    ChatServer(const std::string& host, int port);

    /**
     * Destructor - ensures proper cleanup
     */
    ~ChatServer();

    /**
     * Start the server
     * Creates listening socket and accepts connections
     * @return true on success, false on error
     */
    bool start();

    /**
     * Stop the server gracefully
     * Closes all client connections and listening socket
     */
    void stop();

    /**
     * Broadcast message to all connected clients except one
     * Thread-safe operation
     * @param msg Message to broadcast
     * @param exclude_client_id Client ID to exclude from broadcast
     */
    void broadcast_message(const Message& msg, int exclude_client_id);

    /**
     * Add a client to the active clients list
     * Thread-safe operation
     * @param client_id Client identifier
     * @param socket_fd Socket file descriptor
     * @param username Client's username
     */
    void add_client(int client_id, int socket_fd, const std::string& username);

    /**
     * Remove a client from the active clients list
     * Thread-safe operation
     * @param client_id Client identifier
     */
    void remove_client(int client_id);

private:
    /**
     * Accept loop - runs in main thread
     * Accepts incoming connections and spawns client handlers
     */
    void accept_loop();

    // Server configuration
    std::string host_;
    int port_;
    int server_fd_;

    // Client management
    struct ClientInfo {
        int socket_fd;
        std::string username;
        std::thread handler_thread;
    };

    std::map<int, ClientInfo> clients_;  // client_id -> ClientInfo
    std::mutex clients_mutex_;           // Protects clients_ map
    int next_client_id_;                 // Auto-incrementing client ID

    // Server state
    std::atomic<bool> running_;
};

#endif // SERVER_H
