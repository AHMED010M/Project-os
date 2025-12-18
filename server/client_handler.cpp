// MIT License
// Multi-threaded Chat System - Client Handler Implementation
// Copyright (c) 2025

#include "client_handler.h"
#include "server.h"
#include "common.h"
#include <unistd.h>
#include <cstring>

ClientHandler::ClientHandler(int socket_fd, int client_id, ChatServer* server)
    : socket_fd_(socket_fd)
    , client_id_(client_id)
    , server_(server)
    , should_stop_(false) {
}

ClientHandler::~ClientHandler() {
    cleanup();
}

void ClientHandler::run() {
    LOG_INFO("Client handler " << client_id_ << " started");

    // Receive username first
    if (!receive_username()) {
        LOG_ERROR("Failed to receive username from client " << client_id_);
        cleanup();
        return;
    }

    LOG_INFO("Client " << client_id_ << " username: " << username_);

    // Enter message loop
    message_loop();

    // Cleanup when done
    cleanup();
}

bool ClientHandler::receive_username() {
    Message msg;
    
    // Receive first message (username)
    if (!ChatUtils::recv_message(socket_fd_, msg)) {
        LOG_ERROR("Failed to receive username message");
        return false;
    }

    // Extract username from message text
    username_ = std::string(msg.text);
    
    if (username_.empty()) {
        LOG_ERROR("Received empty username");
        return false;
    }

    return true;
}

void ClientHandler::message_loop() {
    Message msg;
    
    while (!should_stop_) {
        // Receive message from client
        if (!ChatUtils::recv_message(socket_fd_, msg)) {
            // Connection closed or error
            LOG_INFO("Client " << client_id_ << " (" << username_ << ") disconnected");
            break;
        }

        // Update timestamp on server side
        std::string timestamp = Message::get_current_timestamp();
        strncpy(msg.timestamp, timestamp.c_str(), MAX_TIMESTAMP_LEN - 1);
        msg.timestamp[MAX_TIMESTAMP_LEN - 1] = '\0';

        // Ensure username is set correctly
        strncpy(msg.username, username_.c_str(), MAX_USERNAME_LEN - 1);
        msg.username[MAX_USERNAME_LEN - 1] = '\0';

        LOG_INFO("Broadcasting message from " << username_ 
                 << ": " << msg.text);

        // Broadcast to all clients except sender
        server_->broadcast_message(msg, client_id_);
    }
}

void ClientHandler::cleanup() {
    // Remove from server's client list
    server_->remove_client(client_id_);
    
    // Close socket if still open
    if (socket_fd_ >= 0) {
        shutdown(socket_fd_, SHUT_RDWR);
        close(socket_fd_);
        socket_fd_ = -1;
    }

    LOG_INFO("Client handler " << client_id_ << " cleaned up");
}
