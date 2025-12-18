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
 */
void signal_handler(int signum) {
    if (signum == SIGINT) {
        LOG_INFO("\nReceived SIGINT, shutting down gracefully...");
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
        port = std::atoi(argv[2]);
        if (port <= 0 || port > 65535) {
            LOG_ERROR("Invalid port number: " << port);
            return 1;
        }
    }

    // Print banner
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║     Multi-threaded Chat Server        ║\n";
    std::cout << "║           Version 1.0.0               ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";

    // Create server instance
    g_server = new ChatServer(host, port);

    // Setup signal handler
    std::signal(SIGINT, signal_handler);

    // Start server
    if (!g_server->start()) {
        LOG_ERROR("Failed to start server");
        delete g_server;
        return 1;
    }

    // Cleanup
    delete g_server;
    g_server = nullptr;

    LOG_INFO("Server shutdown complete");
    return 0;
}
