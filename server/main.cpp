// MIT License
// Multi-threaded Chat System - Server Entry Point
// Copyright (c) 2025

#include "server.h"
#include "common.h"
#include <csignal>
#include <iostream>

// Global server instance for signal handler
ChatServer* g_server = nullptr;

/**
 * Signal handler for graceful shutdown
 * Handles SIGINT (Ctrl+C)
 */
void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n";
        LOG_INFO("Received SIGINT, shutting down gracefully...");
        if (g_server) {
            g_server->stop();
        }
    }
}

int main(int argc, char* argv[]) {
    // Default configuration
    std::string host = "0.0.0.0";
    int port = 5000;

    // Parse command line arguments
    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = std::stoi(argv[2]);
    }

    // Print banner
    std::cout << "========================================" << std::endl;
    std::cout << "   Multi-threaded Chat Server v1.0    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Create server instance
    ChatServer server(host, port);
    g_server = &server;

    // Setup signal handler
    std::signal(SIGINT, signal_handler);

    // Start server
    LOG_INFO("Chat server starting on " << host << ":" << port << "...");
    
    if (!server.start()) {
        LOG_ERROR("Failed to start server");
        return 1;
    }

    LOG_INFO("Server stopped successfully");
    return 0;
}
