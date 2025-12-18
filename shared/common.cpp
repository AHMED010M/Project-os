// MIT License
// Multi-threaded Chat System - Common Utilities Implementation
// Copyright (c) 2025

#include "common.h"
#include <fcntl.h>
#include <sys/time.h>

namespace ChatUtils {

bool send_message(int socket_fd, const Message& msg) {
    if (socket_fd < 0) {
        LOG_ERROR("Invalid socket descriptor");
        return false;
    }

    const char* data = reinterpret_cast<const char*>(&msg);
    size_t total_sent = 0;
    size_t total_size = sizeof(Message);

    // Send all data, handling partial sends
    while (total_sent < total_size) {
        ssize_t sent = send(socket_fd, data + total_sent, 
                           total_size - total_sent, MSG_NOSIGNAL);
        
        if (sent < 0) {
            if (errno == EINTR) {
                // Interrupted by signal, retry
                continue;
            }
            LOG_ERROR("Send failed: " << strerror(errno));
            return false;
        }
        
        if (sent == 0) {
            LOG_WARN("Connection closed while sending");
            return false;
        }
        
        total_sent += sent;
    }

    return true;
}

bool recv_message(int socket_fd, Message& msg, int timeout_sec) {
    if (socket_fd < 0) {
        LOG_ERROR("Invalid socket descriptor");
        return false;
    }

    // Use select for timeout if specified
    if (timeout_sec > 0) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(socket_fd, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = 0;

        int select_result = select(socket_fd + 1, &read_fds, nullptr, nullptr, &timeout);
        
        if (select_result < 0) {
            if (errno != EINTR) {
                LOG_ERROR("Select failed: " << strerror(errno));
            }
            return false;
        }
        
        if (select_result == 0) {
            // Timeout occurred
            return false;
        }
    }

    char* data = reinterpret_cast<char*>(&msg);
    size_t total_received = 0;
    size_t total_size = sizeof(Message);

    // Receive all data, handling partial receives
    while (total_received < total_size) {
        ssize_t received = recv(socket_fd, data + total_received, 
                               total_size - total_received, 0);
        
        if (received < 0) {
            if (errno == EINTR) {
                // Interrupted by signal, retry
                continue;
            }
            if (errno != ECONNRESET) {
                LOG_ERROR("Receive failed: " << strerror(errno));
            }
            return false;
        }
        
        if (received == 0) {
            // Connection closed by peer
            if (total_received == 0) {
                // Clean disconnect
                return false;
            } else {
                LOG_WARN("Connection closed during receive");
                return false;
            }
        }
        
        total_received += received;
    }

    // Validate received message
    if (!msg.is_valid()) {
        LOG_WARN("Received invalid message");
        return false;
    }

    return true;
}

bool set_nonblocking(int socket_fd) {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags < 0) {
        LOG_ERROR("fcntl F_GETFL failed: " << strerror(errno));
        return false;
    }

    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        LOG_ERROR("fcntl F_SETFL failed: " << strerror(errno));
        return false;
    }

    return true;
}

bool set_socket_options(int socket_fd) {
    // Enable address reuse (allows quick restart)
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        LOG_WARN("Failed to set SO_REUSEADDR: " << strerror(errno));
        // Not critical, continue
    }

    // Enable keepalive (detect dead connections)
    opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
        LOG_WARN("Failed to set SO_KEEPALIVE: " << strerror(errno));
        // Not critical, continue
    }

    return true;
}

} // namespace ChatUtils
