// MIT License
// Multi-threaded Chat System - Socket Client Header
// Copyright (c) 2025

#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QString>
#include <atomic>
#include <thread>

/**
 * TCP socket client for chat communication
 * Uses separate thread for receiving messages
 */
class SocketClient : public QObject {
    Q_OBJECT

public:
    explicit SocketClient(QObject* parent = nullptr);
    ~SocketClient();

    /**
     * Connect to chat server
     * @param host Server IP address
     * @param port Server port
     * @param username Client username
     */
    void connect_to_server(const QString& host, int port, const QString& username);

    /**
     * Disconnect from server
     * Non-blocking, safe to call from GUI thread
     */
    void disconnect_from_server();

    /**
     * Send a message to the server
     * @param text Message text
     * @return true on success
     */
    bool send_message(const QString& text);

    /**
     * Check if connected
     */
    bool is_connected() const { return connected_; }

signals:
    /**
     * Emitted when a message is received
     */
    void message_received(const QString& username, 
                         const QString& timestamp, 
                         const QString& text);

    /**
     * Emitted when connection is established
     */
    void connected();

    /**
     * Emitted when disconnected
     */
    void disconnected();

    /**
     * Emitted on error
     */
    void error_occurred(const QString& error);

private:
    int socket_fd_;                         // Socket file descriptor
    std::atomic<bool> connected_;           // Connection state
    std::atomic<bool> should_stop_;         // Stop flag for receive thread
    std::thread receive_thread_;            // Receive thread
    QString username_;                      // Client username

    /**
     * Receive loop - runs in separate thread
     * Continuously receives messages from server
     */
    void receive_loop();

    /**
     * Connect to server (internal implementation)
     */
    bool do_connect(const QString& host, int port);

    /**
     * Send username to server (first message)
     */
    bool send_username();
};

#endif // SOCKETCLIENT_H
