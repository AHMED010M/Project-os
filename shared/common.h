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

// Logging macros with colors
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

/**
 * Namespace for chat utility functions
 */
namespace ChatUtils {

/**
 * Send a complete message over a socket
 * Handles partial sends automatically
 * 
 * @param socket_fd File descriptor of the socket
 * @param msg Message to send
 * @return true if entire message sent successfully, false otherwise
 */
bool send_message(int socket_fd, const Message& msg);

/**
 * Receive a complete message from a socket
 * Handles partial receives automatically
 * Uses select() for timeout support
 * 
 * @param socket_fd File descriptor of the socket
 * @param msg Reference to Message struct to fill
 * @param timeout_sec Timeout in seconds (0 = no timeout)
 * @return true if message received successfully, false on error or disconnect
 */
bool recv_message(int socket_fd, Message& msg, int timeout_sec = 0);

/**
 * Set socket to non-blocking mode
 * 
 * @param socket_fd File descriptor of the socket
 * @return true on success, false on error
 */
bool set_nonblocking(int socket_fd);

/**
 * Set socket options for better performance
 * - SO_REUSEADDR: Allow quick restart
 * - SO_KEEPALIVE: Detect dead connections
 * 
 * @param socket_fd File descriptor of the socket
 * @return true on success, false on error
 */
bool set_socket_options(int socket_fd);

} // namespace ChatUtils

#endif // COMMON_H
