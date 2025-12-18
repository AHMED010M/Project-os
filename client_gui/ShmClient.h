#ifndef SHM_CLIENT_H
#define SHM_CLIENT_H

#include <QObject>
#include <QString>
#include <atomic>
#include <thread>
#include "../shared/protocol.h"

const size_t SHM_BUFFER_SIZE = 64;

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

    bool join_room(const QString& shm_name, const QString& username);
    void leave_room();
    bool is_joined() const { return joined_; }
    bool send_message(const QString& text);

signals:
    void message_received(QString user, QString timestamp, QString text);
    void connected();
    void disconnected();
    void error_occurred(QString error_msg);

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

    void* write_sem_;
    void* read_sem_;
};

#endif
