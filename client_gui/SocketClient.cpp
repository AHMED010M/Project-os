#include "SocketClient.h"
#include "../shared/common.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstddef>
#include <QDebug>

using namespace ChatUtils;

SocketClient::SocketClient(QObject* parent)
    : QObject(parent), socket_fd_(-1), connected_(false), should_stop_(false) {}

SocketClient::~SocketClient() {
    disconnect();
}

bool SocketClient::connect_to_server(const QString& host, int port, const QString& username) {
    if (connected_) {
        emit error_occurred("Already connected");
        return false;
    }

    if (username.trimmed().isEmpty()) {
        emit error_occurred("Username cannot be empty");
        return false;
    }

    username_ = username;

    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        emit error_occurred("Failed to create socket");
        return false;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.toStdString().c_str(), &server_addr.sin_addr) <= 0) {
        emit error_occurred("Invalid server address");
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    if (::connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        emit error_occurred("Failed to connect to server");
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    Message msg;
    std::string username_str = username_.toStdString();
    strncpy(msg.text, username_str.c_str(), MAX_MESSAGE_LEN - 1);
    msg.text[MAX_MESSAGE_LEN - 1] = '\0';
    strncpy(msg.username, username_str.c_str(), MAX_USERNAME_LEN - 1);
    msg.username[MAX_USERNAME_LEN - 1] = '\0';
    strncpy(msg.timestamp, Message::get_current_timestamp().c_str(), MAX_TIMESTAMP_LEN - 1);
    msg.timestamp[MAX_TIMESTAMP_LEN - 1] = '\0';

    if (!ChatUtils::send_message(socket_fd_, msg)) {
        emit error_occurred("Failed to send username");
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    connected_ = true;
    should_stop_ = false;
    receive_thread_ = std::thread(&SocketClient::receive_loop, this);

    emit connected();
    return true;
}

void SocketClient::disconnect() {
    if (!connected_) return;

    should_stop_ = true;
    connected_ = false;

    if (socket_fd_ >= 0) {
        shutdown(socket_fd_, SHUT_RDWR);
        close(socket_fd_);
        socket_fd_ = -1;
    }

    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }

    emit disconnected();
}

bool SocketClient::send_message(const QString& text) {
    if (!connected_) return false;

    Message msg;
    std::string username_str = username_.toStdString();
    std::string text_str = text.toStdString();
    
    strncpy(msg.username, username_str.c_str(), MAX_USERNAME_LEN - 1);
    msg.username[MAX_USERNAME_LEN - 1] = '\0';
    strncpy(msg.timestamp, Message::get_current_timestamp().c_str(), MAX_TIMESTAMP_LEN - 1);
    msg.timestamp[MAX_TIMESTAMP_LEN - 1] = '\0';
    strncpy(msg.text, text_str.c_str(), MAX_MESSAGE_LEN - 1);
    msg.text[MAX_MESSAGE_LEN - 1] = '\0';

    return ChatUtils::send_message(socket_fd_, msg);
}

void SocketClient::receive_loop() {
    Message msg;
    while (!should_stop_ && ChatUtils::recv_message(socket_fd_, msg)) {
        emit message_received(
            QString::fromUtf8(msg.username),
            QString::fromUtf8(msg.timestamp),
            QString::fromUtf8(msg.text)
        );
    }

    if (connected_) {
        connected_ = false;
        emit disconnected();
    }
}
