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
#include <algorithm>

ChatServer::ChatServer(const std::string& host, int port)
    : host_(host), port_(port), server_fd_(-1), next_client_id_(1), running_(false) {
}

ChatServer::~ChatServer() {
    stop();
}

bool ChatServer::start() {
    // Create socket
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        LOG_ERROR("Failed to create socket: " << strerror(errno));
        return false;
    }

    // Set socket options
    if (!ChatUtils::set_socket_options(server_fd_)) {
        LOG_WARN("Failed to set socket options (non-critical)");
    }

    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    
    if (host_ == "0.0.0.0") {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, host_.c_str(), &server_addr.sin_addr) <= 0) {
            LOG_ERROR("Invalid address: " << host_);
            close(server_fd_);
            return false;
        }
    }

    if (bind(server_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        LOG_ERROR("Bind failed: " << strerror(errno));
        close(server_fd_);
        return false;
    }

    // Listen
    if (listen(server_fd_, 10) < 0) {
        LOG_ERROR("Listen failed: " << strerror(errno));
        close(server_fd_);
        return false;
    }

    LOG_INFO("Chat server starting on " << host_ << ":" << port_ << "...");
    LOG_INFO("Server is running. Press Ctrl+C to stop.");

    running_ = true;
    accept_loop();

    return true;
}

void ChatServer::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    // Close server socket
    if (server_fd_ >= 0) {
        shutdown(server_fd_, SHUT_RDWR);
        close(server_fd_);
        server_fd_ = -1;
    }

    // Close all client connections
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto& pair : clients_) {
        if (pair.second.socket_fd >= 0) {
            shutdown(pair.second.socket_fd, SHUT_RDWR);
            close(pair.second.socket_fd);
        }
        if (pair.second.handler_thread.joinable()) {
            pair.second.handler_thread.detach();
        }
    }
    clients_.clear();
}

void ChatServer::accept_loop() {
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_addr_len);
        
        if (client_fd < 0) {
            if (running_) {
                LOG_ERROR("Accept failed: " << strerror(errno));
            }
            break;
        }

        // Get client info
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        int client_id = next_client_id_++;
        LOG_INFO("Client connected: ID " << client_id << " from " << client_ip << ":" << client_port);

        // Create client handler thread
        ClientHandler* handler = new ClientHandler(client_fd, client_id, this);
        
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_[client_id] = ClientInfo{client_fd, "", std::thread(&ClientHandler::run, handler)};
    }
}

void ChatServer::broadcast_message(const Message& msg, int exclude_client_id) {
    std::lock_guard<std::mutex> lock(clients_mutex_);

    LOG_INFO("Broadcasting message from " << msg.username << " to " 
             << (clients_.size() - 1) << " clients");

    for (auto& pair : clients_) {
        if (pair.first == exclude_client_id) {
            continue;  // Don't send to sender
        }

        if (!ChatUtils::send_message(pair.second.socket_fd, msg)) {
            LOG_WARN("Failed to send message to client " << pair.first);
        }
    }
}

void ChatServer::add_client(int client_id, int socket_fd, const std::string& username) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    
    auto it = clients_.find(client_id);
    if (it != clients_.end()) {
        it->second.username = username;
        LOG_INFO("Client " << client_id << " username: " << username);
    }
}

void ChatServer::remove_client(int client_id) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    
    auto it = clients_.find(client_id);
    if (it != clients_.end()) {
        LOG_INFO("Client disconnected: ID " << client_id << " (" << it->second.username << ")");
        
        if (it->second.socket_fd >= 0) {
            close(it->second.socket_fd);
        }
        
        if (it->second.handler_thread.joinable()) {
            it->second.handler_thread.detach();
        }
        
        clients_.erase(it);
    }
}
