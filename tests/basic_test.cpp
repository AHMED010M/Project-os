/*
 * MIT License
 * Copyright (c) 2025 OS Chat Project
 *
 * Basic functionality tests
 */

#include "../shared/protocol.h"
#include "../shared/common.h"
#include <iostream>
#include <cassert>
#include <cstring>

void test_message_creation() {
    std::cout << "Testing Message creation..." << std::endl;
    
    Message msg;
    strncpy(msg.user, "TestUser", MAX_USERNAME_LEN - 1);
    strncpy(msg.text, "Hello, World!", MAX_MESSAGE_LEN - 1);
    strncpy(msg.timestamp, Message::get_current_timestamp().c_str(), MAX_TIMESTAMP_LEN - 1);
    
    assert(std::string(msg.user) == "TestUser");
    assert(std::string(msg.text) == "Hello, World!");
    assert(strlen(msg.timestamp) > 0);
    
    std::cout << "  ✓ Message creation test passed" << std::endl;
}

void test_timestamp_generation() {
    std::cout << "Testing timestamp generation..." << std::endl;
    
    std::string ts1 = Message::get_current_timestamp();
    std::string ts2 = Message::get_current_timestamp();
    
    assert(!ts1.empty());
    assert(!ts2.empty());
    assert(ts1.length() >= 19);  // ISO 8601 format minimum length
    
    std::cout << "  ✓ Timestamp: " << ts1 << std::endl;
    std::cout << "  ✓ Timestamp generation test passed" << std::endl;
}

void test_message_size() {
    std::cout << "Testing Message structure size..." << std::endl;
    
    size_t msg_size = sizeof(Message);
    std::cout << "  Message size: " << msg_size << " bytes" << std::endl;
    
    // Ensure message is not too large for network transmission
    assert(msg_size < 1024);  // Should be less than 1KB
    
    std::cout << "  ✓ Message size test passed" << std::endl;
}

void test_max_lengths() {
    std::cout << "Testing maximum field lengths..." << std::endl;
    
    Message msg;
    
    // Test username max length
    std::string long_username(MAX_USERNAME_LEN + 10, 'A');
    strncpy(msg.user, long_username.c_str(), MAX_USERNAME_LEN - 1);
    msg.user[MAX_USERNAME_LEN - 1] = '\0';
    assert(strlen(msg.user) < MAX_USERNAME_LEN);
    
    // Test message text max length
    std::string long_text(MAX_MESSAGE_LEN + 10, 'B');
    strncpy(msg.text, long_text.c_str(), MAX_MESSAGE_LEN - 1);
    msg.text[MAX_MESSAGE_LEN - 1] = '\0';
    assert(strlen(msg.text) < MAX_MESSAGE_LEN);
    
    std::cout << "  ✓ Max lengths test passed" << std::endl;
}

void test_message_copy() {
    std::cout << "Testing Message copy..." << std::endl;
    
    Message msg1;
    strncpy(msg1.user, "Alice", MAX_USERNAME_LEN - 1);
    strncpy(msg1.text, "Test message", MAX_MESSAGE_LEN - 1);
    strncpy(msg1.timestamp, "2025-12-18T12:00:00Z", MAX_TIMESTAMP_LEN - 1);
    
    Message msg2 = msg1;
    
    assert(std::string(msg2.user) == "Alice");
    assert(std::string(msg2.text) == "Test message");
    assert(std::string(msg2.timestamp) == "2025-12-18T12:00:00Z");
    
    std::cout << "  ✓ Message copy test passed" << std::endl;
}

int main() {
    std::cout << "=" << std::string(50, '=') << std::endl;
    std::cout << "OS Chat Project - Basic Tests" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl << std::endl;
    
    try {
        test_message_creation();
        test_timestamp_generation();
        test_message_size();
        test_max_lengths();
        test_message_copy();
        
        std::cout << std::endl;
        std::cout << "=" << std::string(50, '=') << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        std::cout << "=" << std::string(50, '=') << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
