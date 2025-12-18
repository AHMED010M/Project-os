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
#include <memory>

// Forward declaration
class ClientHandler;

/**
 * Multi-threaded TCP chat server
 * Manages client connections and message broadcasting
 */
class ChatServer {
public:
    /**
     * Constructor
     * @param host IP address to bind to
     * @param port Port to listen on
     */
    ChatServer(const std::string& host, int port);
    
    /**
     * Destructor - ensures clean shutdown
     */
    ~ChatServer();

    /**
     * Start the server
     * Blocks until server is stopped
     * @return true on success, false on error
     */
    bool start();

    /**
     * Stop the server gracefully
     * Closes all client connections
     */
    void stop();

    /**
     * Broadcast message to all clients except one
     * @param msg Message to broadcast
     * @param exclude_client_id Client ID to exclude (sender)
     */
    void broadcast_message(const Message& msg, int exclude_client_id);

    /**
     * Add a client to the active clients list
     * @param client_id Unique client identifier
     * @param socket_fd Client socket file descriptor
     */
    void add_client(int client_id, int socket_fd);

    /**
     * Remove a client from the active clients list
     * @param client_id Client identifier to remove
     */
    void remove_client(int client_id);

private:
    std::string host_;                              // Server host
    int port_;                                      // Server port
    int server_fd_;                                 // Server socket descriptor
    std::atomic<bool> running_;                     // Server running flag
    
    std::map<int, int> clients_;                    // client_id -> socket_fd
    std::mutex clients_mutex_;                      // Mutex for clients map
    
    int next_client_id_;                            // Next available client ID
    std::mutex id_mutex_;                           // Mutex for client ID generation

    /**
     * Accept loop - runs in main thread
     * Accepts incoming connections and spawns client handlers
     */
    void accept_loop();

    /**
     * Create and bind server socket
     * @return true on success, false on error
     */
    bool create_socket();

    /**
     * Get next available client ID (thread-safe)
     * @return Unique client ID
     */
    int get_next_client_id();
};

#endif // SERVER_H
