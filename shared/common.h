// MIT License
// Multi-threaded Chat System - Common Utilities
// Copyright (c) 2025

#ifndef COMMON_H
#define COMMON_H

#include "protocol.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <cstring>
#include <cerrno>
#include <cstddef>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG_INFO(msg) \
    std::cout << ANSI_COLOR_GREEN << "[INFO] " << ANSI_COLOR_RESET << msg << std::endl

#define LOG_WARN(msg) \
    std::cout << ANSI_COLOR_YELLOW << "[WARN] " << ANSI_COLOR_RESET << msg << std::endl

#define LOG_ERROR(msg) \
    std::cerr << ANSI_COLOR_RED << "[ERROR] " << ANSI_COLOR_RESET << msg << std::endl

namespace ChatUtils {

bool send_message(int socket_fd, const Message& msg);
bool recv_message(int socket_fd, Message& msg, int timeout_sec = 0);
bool set_nonblocking(int socket_fd);
bool set_socket_options(int socket_fd);

}

#endif // COMMON_H
