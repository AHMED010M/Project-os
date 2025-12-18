#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QString>
#include <atomic>
#include <thread>
#include "../shared/protocol.h"

class SocketClient : public QObject {
    Q_OBJECT

public:
    explicit SocketClient(QObject* parent = nullptr);
    ~SocketClient();

    bool connect_to_server(const QString& host, int port, const QString& username);
    void disconnect();
    bool send_message(const QString& text);
    bool is_connected() const { return connected_; }

signals:
    void message_received(const QString& username, const QString& timestamp, const QString& text);
    void connected();
    void disconnected();
    void error_occurred(const QString& error);

private:
    int socket_fd_;
    std::atomic<bool> connected_;
    std::atomic<bool> should_stop_;
    std::thread receive_thread_;
    QString username_;

    void receive_loop();
};

#endif
