#include "ShmClient.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>
#include <QDebug>

ShmClient::ShmClient(QObject* parent)
    : QObject(parent), shm_fd_(-1), shm_buffer_(nullptr), 
      last_read_index_(0), joined_(false), should_stop_(false),
      write_sem_(nullptr), read_sem_(nullptr) {}

ShmClient::~ShmClient() {
    leave_room();
}

bool ShmClient::join_room(const QString& shm_name, const QString& username) {
    if (joined_) {
        emit error_occurred("Already joined");
        return false;
    }

    shm_name_ = shm_name;
    username_ = username;

    if (!create_or_open_shm(shm_name)) {
        emit error_occurred("Failed to create/open shared memory");
        return false;
    }

    QString write_sem_name = shm_name + "_write";
    QString read_sem_name = shm_name + "_read";

    write_sem_ = sem_open(write_sem_name.toStdString().c_str(), O_CREAT, 0644, 1);
    read_sem_ = sem_open(read_sem_name.toStdString().c_str(), O_CREAT, 0644, 1);

    if (write_sem_ == SEM_FAILED || read_sem_ == SEM_FAILED) {
        emit error_occurred("Failed to create semaphores");
        cleanup_shm();
        return false;
    }

    joined_ = true;
    should_stop_ = false;
    last_read_index_ = shm_buffer_->read_index;
    read_thread_ = std::thread(&ShmClient::read_loop, this);

    emit connected();
    return true;
}

void ShmClient::leave_room() {
    if (!joined_) return;

    should_stop_ = true;
    joined_ = false;

    if (read_thread_.joinable()) {
        read_thread_.join();
    }

    cleanup_shm();
    emit disconnected();
}

bool ShmClient::send_message(const QString& text) {
    if (!joined_ || !shm_buffer_) return false;

    sem_wait((sem_t*)write_sem_);

    Message msg;
    strncpy(msg.username, username_.toStdString().c_str(), MAX_USERNAME_LEN - 1);
    strncpy(msg.timestamp, Message::get_current_timestamp().c_str(), MAX_TIMESTAMP_LEN - 1);
    strncpy(msg.text, text.toStdString().c_str(), MAX_MESSAGE_LEN - 1);

    size_t write_idx = shm_buffer_->write_index % SHM_BUFFER_SIZE;
    shm_buffer_->messages[write_idx] = msg;
    shm_buffer_->write_index++;

    sem_post((sem_t*)write_sem_);
    return true;
}

void ShmClient::read_loop() {
    while (!should_stop_) {
        sem_wait((sem_t*)read_sem_);

        size_t current_write_index = shm_buffer_->write_index;

        while (last_read_index_ < current_write_index) {
            size_t read_idx = last_read_index_ % SHM_BUFFER_SIZE;
            Message msg = shm_buffer_->messages[read_idx];

            if (QString::fromUtf8(msg.username) != username_) {
                emit message_received(
                    QString::fromUtf8(msg.username),
                    QString::fromUtf8(msg.timestamp),
                    QString::fromUtf8(msg.text)
                );
            }

            last_read_index_++;
        }

        sem_post((sem_t*)read_sem_);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool ShmClient::create_or_open_shm(const QString& shm_name) {
    shm_fd_ = shm_open(shm_name.toStdString().c_str(), 
                       O_CREAT | O_RDWR, 
                       S_IRUSR | S_IWUSR);
    
    if (shm_fd_ < 0) {
        return false;
    }

    if (ftruncate(shm_fd_, sizeof(ShmBuffer)) < 0) {
        close(shm_fd_);
        return false;
    }

    shm_buffer_ = (ShmBuffer*)mmap(nullptr, sizeof(ShmBuffer),
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, shm_fd_, 0);
    
    if (shm_buffer_ == MAP_FAILED) {
        close(shm_fd_);
        return false;
    }

    if (shm_buffer_->write_index == 0 && shm_buffer_->read_index == 0) {
        std::memset(shm_buffer_, 0, sizeof(ShmBuffer));
    }

    return true;
}

void ShmClient::cleanup_shm() {
    if (shm_buffer_) {
        munmap(shm_buffer_, sizeof(ShmBuffer));
        shm_buffer_ = nullptr;
    }

    if (shm_fd_ >= 0) {
        close(shm_fd_);
        shm_fd_ = -1;
    }

    if (write_sem_) {
        sem_close((sem_t*)write_sem_);
        write_sem_ = nullptr;
    }

    if (read_sem_) {
        sem_close((sem_t*)read_sem_);
        read_sem_ = nullptr;
    }
}
