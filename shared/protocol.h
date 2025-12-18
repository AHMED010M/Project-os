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
#include <arpa/inet.h>

// Protocol constants
const int MAX_USERNAME_LEN = 32;
const int MAX_TIMESTAMP_LEN = 32;
const int MAX_MESSAGE_LEN = 512;

/**
 * Message structure for chat protocol
 * Total size: 576 bytes (fixed size for easy serialization)
 */
struct Message {
    char username[MAX_USERNAME_LEN];      // Username of sender
    char timestamp[MAX_TIMESTAMP_LEN];    // ISO 8601 timestamp
    char text[MAX_MESSAGE_LEN];           // Message content

    // Constructor
    Message() {
        memset(username, 0, MAX_USERNAME_LEN);
        memset(timestamp, 0, MAX_TIMESTAMP_LEN);
        memset(text, 0, MAX_MESSAGE_LEN);
    }

    /**
     * Get current timestamp in ISO 8601 format
     * Format: YYYY-MM-DDTHH:MM:SSZ
     */
    static std::string get_current_timestamp() {
        std::time_t now = std::time(nullptr);
        std::tm* utc_time = std::gmtime(&now);
        
        std::ostringstream oss;
        oss << std::put_time(utc_time, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }

    /**
     * Convert message to network byte order
     * (Currently no multi-byte integers, but kept for future extensions)
     */
    void to_network_order() {
        // Message structure uses only char arrays
        // No conversion needed for current implementation
        // This function is a placeholder for future extensions
    }

    /**
     * Convert message from network byte order
     * (Currently no multi-byte integers, but kept for future extensions)
     */
    void from_network_order() {
        // Message structure uses only char arrays
        // No conversion needed for current implementation
        // This function is a placeholder for future extensions
    }

    /**
     * Validate message content
     * Returns true if message is valid
     */
    bool is_valid() const {
        // Check username is not empty
        if (username[0] == '\0') {
            return false;
        }
        
        // Check text is not empty
        if (text[0] == '\0') {
            return false;
        }
        
        // Check strings are null-terminated
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

    /**
     * Clear all message data
     */
    void clear() {
        memset(username, 0, MAX_USERNAME_LEN);
        memset(timestamp, 0, MAX_TIMESTAMP_LEN);
        memset(text, 0, MAX_MESSAGE_LEN);
    }
};

#endif // PROTOCOL_H
