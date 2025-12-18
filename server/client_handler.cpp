// MIT License
// Multi-threaded Chat System - Client Handler Implementation
// Copyright (c) 2025

#include "client_handler.h"
#include "server.h"
#include "common.h"
#include <unistd.h>

ClientHandler::ClientHandler(int socket_fd, int client_id, ChatServer* server)
    : socket_fd_(socket_fd), client_id_(client_id), server_(server), should_stop_(false) {
}

ClientHandler::~ClientHandler() {
    should_stop_ = true;
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

void ClientHandler::run() {
    // First message must be username
    if (!receive_username()) {
        LOG_ERROR("Failed to receive username from client " << client_id_);
        server_->remove_client(client_id_);
        delete this;
        return;
    }

    // Add client to server's client list
    server_->add_client(client_id_, socket_fd_, username_);

    // Enter message loop
    message_loop();

    // Cleanup
    server_->remove_client(client_id_);
    delete this;
}

bool ClientHandler::receive_username() {
    Message msg;
    
    if (!ChatUtils::recv_message(socket_fd_, msg)) {
        return false;
    }

    // First message text is the username
    username_ = msg.text;
    
    if (username_.empty()) {
        LOG_WARN("Client " << client_id_ << " sent empty username");
        return false;
    }

    return true;
}

void ClientHandler::message_loop() {
    Message msg;
    
    while (!should_stop_ && ChatUtils::recv_message(socket_fd_, msg)) {
        // Update timestamp on server side
        std::string timestamp = Message::get_current_timestamp();
        strncpy(msg.timestamp, timestamp.c_str(), MAX_TIMESTAMP_LEN - 1);
        msg.timestamp[MAX_TIMESTAMP_LEN - 1] = '\0';

        // Set username (in case client didn't set it correctly)
        strncpy(msg.username, username_.c_str(), MAX_USERNAME_LEN - 1);
        msg.username[MAX_USERNAME_LEN - 1] = '\0';

        // Broadcast to all other clients
        server_->broadcast_message(msg, client_id_);
    }
}
