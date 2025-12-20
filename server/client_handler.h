// MIT License
// Multi-threaded Chat System - Client Handler Header
// Copyright (c) 2025

#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "protocol.h"
#include <atomic>
#include <string>

class ChatServer;

class ClientHandler {
public:
    ClientHandler(int socket_fd, int client_id, ChatServer* server);
    ~ClientHandler();

    void run();

private:
    bool receive_username();
    void message_loop();

    int socket_fd_;
    int client_id_;
    ChatServer* server_;
    std::string username_;
    std::atomic<bool> should_stop_;
};

#endif // CLIENT_HANDLER_H
