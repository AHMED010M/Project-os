// MIT License
// Multi-threaded Chat System - Main Window Header
// Copyright (c) 2025

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <memory>

// Forward declarations
class SocketClient;
class ShmClient;

/**
 * Main window for chat client GUI
 * Supports both Socket and Shared Memory modes
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    /**
     * Handle connection mode change
     */
    void on_mode_changed(int index);

    /**
     * Handle connect/disconnect button click
     */
    void on_connect_clicked();

    /**
     * Handle send button click
     */
    void on_send_clicked();

    /**
     * Handle message received from socket client
     */
    void on_socket_message_received(const QString& username, 
                                   const QString& timestamp, 
                                   const QString& text);

    /**
     * Handle socket connection established
     */
    void on_socket_connected();

    /**
     * Handle socket disconnection
     */
    void on_socket_disconnected();

    /**
     * Handle socket error
     */
    void on_socket_error(const QString& error);

    /**
     * Handle shared memory message received
     */
    void on_shm_message_received(const QString& username,
                                const QString& timestamp,
                                const QString& text);

    /**
     * Handle shared memory connection established
     */
    void on_shm_connected();

    /**
     * Handle shared memory disconnection
     */
    void on_shm_disconnected();

    /**
     * Handle shared memory error
     */
    void on_shm_error(const QString& error);

private:
    // UI Components
    QWidget* central_widget_;
    QVBoxLayout* main_layout_;
    
    // Mode selection
    QGroupBox* mode_group_;
    QComboBox* mode_combo_;
    
    // Connection settings
    QGroupBox* connection_group_;
    QLineEdit* ip_input_;
    QLineEdit* port_input_;
    QLineEdit* shm_name_input_;
    QLineEdit* username_input_;
    QLabel* ip_label_;
    QLabel* port_label_;
    QLabel* shm_name_label_;
    
    // Status
    QLabel* status_label_;
    QPushButton* connect_button_;
    
    // Messages
    QTextEdit* message_display_;
    QLineEdit* message_input_;
    QPushButton* send_button_;
    
    // Client instances
    std::unique_ptr<SocketClient> socket_client_;
    std::unique_ptr<ShmClient> shm_client_;
    
    // State
    bool is_connected_;
    enum Mode { SOCKET, SHARED_MEMORY };
    Mode current_mode_;

    /**
     * Setup UI components
     */
    void setup_ui();

    /**
     * Apply dark theme styling
     */
    void apply_dark_theme();

    /**
     * Update UI based on connection state
     */
    void update_connection_ui();

    /**
     * Update UI based on selected mode
     */
    void update_mode_ui();

    /**
     * Display a message in the message display area
     */
    void display_message(const QString& username,
                        const QString& timestamp,
                        const QString& text);

    /**
     * Display a system message
     */
    void display_system_message(const QString& text);
};

#endif // MAINWINDOW_H
