// MIT License
// Multi-threaded Chat System - Protocol Definition
// Copyright (c) 2025

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstring>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstddef>

const int MAX_USERNAME_LEN = 32;
const int MAX_TIMESTAMP_LEN = 32;
const int MAX_MESSAGE_LEN = 512;

struct Message {
    char username[MAX_USERNAME_LEN];
    char timestamp[MAX_TIMESTAMP_LEN];
    char text[MAX_MESSAGE_LEN];

    Message() {
        memset(username, 0, MAX_USERNAME_LEN);
        memset(timestamp, 0, MAX_TIMESTAMP_LEN);
        memset(text, 0, MAX_MESSAGE_LEN);
    }

    static std::string get_current_timestamp() {
        std::time_t now = std::time(nullptr);
        std::tm* utc_time = std::gmtime(&now);
        
        std::ostringstream oss;
        oss << std::put_time(utc_time, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }

    void to_network_order() {
    }

    void from_network_order() {
    }

    bool is_valid() const {
        if (text[0] == '\0') {
            return false;
        }
        
        if (username[MAX_USERNAME_LEN - 1] != '\0' &&
            strnlen(username, MAX_USERNAME_LEN) == MAX_USERNAME_LEN) {
            return false;
        }
        
        if (text[MAX_MESSAGE_LEN - 1] != '\0' &&
            strnlen(text, MAX_MESSAGE_LEN) == MAX_MESSAGE_LEN) {
            return false;
        }
        
        return true;
    }

    void clear() {
        memset(username, 0, MAX_USERNAME_LEN);
        memset(timestamp, 0, MAX_TIMESTAMP_LEN);
        memset(text, 0, MAX_MESSAGE_LEN);
    }
};

#endif // PROTOCOL_H
