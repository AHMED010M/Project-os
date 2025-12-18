// MIT License
// Multi-threaded Chat System - Server Implementation
// Copyright (c) 2025

#include "server.h"
#include "client_handler.h"
#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>

ChatServer::ChatServer(const std::string& host, int port)
    : host_(host)
    , port_(port)
    , server_fd_(-1)
    , running_(false)
    , next_client_id_(1) {
}

ChatServer::~ChatServer() {
    stop();
}

bool ChatServer::create_socket() {
    // Create socket
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        LOG_ERROR("Failed to create socket: " << strerror(errno));
        return false;
    }

    // Set socket options
    if (!ChatUtils::set_socket_options(server_fd_)) {
        LOG_WARN("Failed to set some socket options");
    }

    // Prepare server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    
    if (host_ == "0.0.0.0") {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, host_.c_str(), &server_addr.sin_addr) <= 0) {
            LOG_ERROR("Invalid IP address: " << host_);
            close(server_fd_);
            server_fd_ = -1;
            return false;
        }
    }

    // Bind socket
    if (bind(server_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        LOG_ERROR("Failed to bind socket: " << strerror(errno));
        close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    // Listen for connections
    if (listen(server_fd_, 10) < 0) {
        LOG_ERROR("Failed to listen: " << strerror(errno));
        close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    LOG_INFO("Server socket created and listening");
    return true;
}

bool ChatServer::start() {
    if (running_) {
        LOG_WARN("Server is already running");
        return false;
    }

    // Create server socket
    if (!create_socket()) {
        return false;
    }

    running_ = true;
    LOG_INFO("Server is running. Press Ctrl+C to stop.");

    // Start accept loop (blocking)
    accept_loop();

    return true;
}

void ChatServer::accept_loop() {
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Accept new connection
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            if (running_) {
                LOG_ERROR("Failed to accept connection: " << strerror(errno));
            }
            continue;
        }

        // Get client info
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        // Generate client ID
        int client_id = get_next_client_id();
        
        LOG_INFO("Client connected: ID " << client_id 
                 << " from " << client_ip << ":" << client_port);

        // Add to clients map
        add_client(client_id, client_fd);

        // Create and start client handler in new thread
        auto handler = std::make_shared<ClientHandler>(client_fd, client_id, this);
        std::thread client_thread(&ClientHandler::run, handler);
        client_thread.detach();  // Detach thread to run independently
    }
}

void ChatServer::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    LOG_INFO("Stopping server...");

    // Close server socket
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }

    // Close all client connections
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto& pair : clients_) {
        close(pair.second);
    }
    clients_.clear();

    LOG_INFO("Server stopped");
}

void ChatServer::broadcast_message(const Message& msg, int exclude_client_id) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    
    for (const auto& pair : clients_) {
        int client_id = pair.first;
        int socket_fd = pair.second;

        // Skip the sender
        if (client_id == exclude_client_id) {
            continue;
        }

        // Send message
        if (!ChatUtils::send_message(socket_fd, msg)) {
            LOG_WARN("Failed to send message to client " << client_id);
        }
    }
}

void ChatServer::add_client(int client_id, int socket_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_[client_id] = socket_fd;
    LOG_INFO("Client " << client_id << " added to active clients list");
}

void ChatServer::remove_client(int client_id) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    auto it = clients_.find(client_id);
    if (it != clients_.end()) {
        close(it->second);
        clients_.erase(it);
        LOG_INFO("Client " << client_id << " removed from active clients list");
    }
}

int ChatServer::get_next_client_id() {
    std::lock_guard<std::mutex> lock(id_mutex_);
    return next_client_id_++;
}
