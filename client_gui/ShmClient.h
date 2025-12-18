/*
 * MIT License
 * Copyright (c) 2025 OS Chat Project
 *
 * Shared Memory Client (System B - Local IPC)
 */

#ifndef SHM_CLIENT_H
#define SHM_CLIENT_H

#include <QObject>
#include <QString>
#include <atomic>
#include <thread>
#include "../shared/protocol.h"

// Shared memory ring buffer configuration
const size_t SHM_BUFFER_SIZE = 64;  // Number of message slots

struct ShmBuffer {
    Message messages[SHM_BUFFER_SIZE];
    size_t write_index;
    size_t read_index;
};

class ShmClient : public QObject {
    Q_OBJECT

public:
    ShmClient(QObject* parent = nullptr);
    ~ShmClient();

    // Join shared memory room
    bool join_room(const QString& shm_name, const QString& username);

    // Leave shared memory room
    void leave_room();

    // Check if joined
    bool is_joined() const { return joined_; }

    // Send a message
    bool send_message(const QString& text);

private:
    void read_loop();
    bool create_or_open_shm(const QString& shm_name);
    void cleanup_shm();

    QString shm_name_;
    QString username_;
    int shm_fd_;
    ShmBuffer* shm_buffer_;
    size_t last_read_index_;

    std::atomic<bool> joined_;
    std::atomic<bool> should_stop_;
    std::thread read_thread_;

    // Semaphores for synchronization
    void* write_sem_;  // Semaphore for write access
    void* read_sem_;   // Semaphore for read access

signals:
    void joined();
    void left();
    void message_received(QString user, QString timestamp, QString text);
    void error_occurred(QString error_msg);
};

#endif  // SHM_CLIENT_H
