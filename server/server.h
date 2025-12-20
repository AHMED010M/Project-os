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
#include <memory>
#include <cstddef>

class ChatServer {
public:
    ChatServer(const std::string& host, int port);
    ~ChatServer();

    bool start();
    void stop();
    void broadcast_message(const Message& msg, int exclude_client_id);
    void add_client(int client_id, int socket_fd, const std::string& username);
    void remove_client(int client_id);

private:
    void accept_loop();

    std::string host_;
    int port_;
    int server_fd_;

    struct ClientInfo {
        int socket_fd;
        std::string username;
        std::thread handler_thread;
    };

    std::map<int, ClientInfo> clients_;
    std::mutex clients_mutex_;
    int next_client_id_;
    std::atomic<bool> running_;
};

#endif // SERVER_H
