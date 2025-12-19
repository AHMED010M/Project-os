#include "MainWindow.h"
#include "SocketClient.h"
#include "ShmClient.h"
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , is_connected_(false)
    , current_mode_(SOCKET) {
    
    setup_ui();
    apply_dark_theme();
    update_connection_ui();
    update_mode_ui();
}

MainWindow::~MainWindow() {
    if (socket_client_) {
        socket_client_->disconnect();
    }
    if (shm_client_) {
        shm_client_->leave_room();
    }
}

void MainWindow::setup_ui() {
    central_widget_ = new QWidget(this);
    setCentralWidget(central_widget_);
    main_layout_ = new QVBoxLayout(central_widget_);

    mode_group_ = new QGroupBox("Connection Mode", this);
    QHBoxLayout* mode_layout = new QHBoxLayout(mode_group_);
    mode_combo_ = new QComboBox(this);
    mode_combo_->addItem("Socket (TCP)");
    mode_combo_->addItem("Shared Memory");
    mode_layout->addWidget(new QLabel("Mode:", this));
    mode_layout->addWidget(mode_combo_);
    mode_layout->addStretch();
    main_layout_->addWidget(mode_group_);

    connection_group_ = new QGroupBox("Connection Settings", this);
    QVBoxLayout* conn_layout = new QVBoxLayout(connection_group_);
    
    QHBoxLayout* ip_layout = new QHBoxLayout();
    ip_label_ = new QLabel("IP Address:", this);
    ip_input_ = new QLineEdit(this);
    ip_input_->setText("127.0.0.1");
    ip_input_->setPlaceholderText("e.g., 127.0.0.1");
    ip_layout->addWidget(ip_label_);
    ip_layout->addWidget(ip_input_);
    conn_layout->addLayout(ip_layout);

    QHBoxLayout* port_layout = new QHBoxLayout();
    port_label_ = new QLabel("Port:", this);
    port_input_ = new QLineEdit(this);
    port_input_->setText("5000");
    port_input_->setPlaceholderText("e.g., 5000");
    port_layout->addWidget(port_label_);
    port_layout->addWidget(port_input_);
    conn_layout->addLayout(port_layout);

    QHBoxLayout* shm_layout = new QHBoxLayout();
    shm_name_label_ = new QLabel("SHM Name:", this);
    shm_name_input_ = new QLineEdit(this);
    shm_name_input_->setText("chat_shm");
    shm_name_input_->setPlaceholderText("e.g., chat_shm");
    shm_layout->addWidget(shm_name_label_);
    shm_layout->addWidget(shm_name_input_);
    conn_layout->addLayout(shm_layout);

    QHBoxLayout* user_layout = new QHBoxLayout();
    user_layout->addWidget(new QLabel("Username:", this));
    username_input_ = new QLineEdit(this);
    username_input_->setPlaceholderText("Enter your username");
    user_layout->addWidget(username_input_);
    conn_layout->addLayout(user_layout);

    main_layout_->addWidget(connection_group_);

    QHBoxLayout* status_layout = new QHBoxLayout();
    status_label_ = new QLabel("Status: Disconnected", this);
    connect_button_ = new QPushButton("Connect", this);
    connect_button_->setFixedWidth(120);
    status_layout->addWidget(status_label_);
    status_layout->addStretch();
    status_layout->addWidget(connect_button_);
    main_layout_->addLayout(status_layout);

    message_display_ = new QTextEdit(this);
    message_display_->setReadOnly(true);
    message_display_->setFont(QFont("Monospace", 11));
    main_layout_->addWidget(message_display_, 1);

    QHBoxLayout* input_layout = new QHBoxLayout();
    message_input_ = new QLineEdit(this);
    message_input_->setPlaceholderText("Type your message here...");
    send_button_ = new QPushButton("Send", this);
    send_button_->setFixedWidth(100);
    send_button_->setEnabled(false);
    input_layout->addWidget(message_input_);
    input_layout->addWidget(send_button_);
    main_layout_->addLayout(input_layout);

    connect(mode_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_mode_changed);
    connect(connect_button_, &QPushButton::clicked,
            this, &MainWindow::on_connect_clicked);
    connect(send_button_, &QPushButton::clicked,
            this, &MainWindow::on_send_clicked);
    connect(message_input_, &QLineEdit::returnPressed,
            this, &MainWindow::on_send_clicked);
}

void MainWindow::apply_dark_theme() {
    QString navy_theme = 
        "QMainWindow {"
        "    background-color: #0a1929;"
        "}"
        "QWidget {"
        "    background-color: #0a1929;"
        "    color: #e3f2fd;"
        "}"
        "QGroupBox {"
        "    background-color: #132f4c;"
        "    border: 2px solid #1e4976;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "    color: #90caf9;"
        "    font-weight: bold;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px;"
        "}"
        "QLineEdit {"
        "    background-color: #1a3a52;"
        "    color: #e3f2fd;"
        "    border: 2px solid #2e5a7d;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    selection-background-color: #1e88e5;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #42a5f5;"
        "}"
        "QTextEdit {"
        "    background-color: #0d2137;"
        "    color: #e3f2fd;"
        "    border: 2px solid #1e4976;"
        "    border-radius: 5px;"
        "    selection-background-color: #1e88e5;"
        "}"
        "QPushButton {"
        "    background-color: #1565c0;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 15px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1976d2;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0d47a1;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #1e3a52;"
        "    color: #546e7a;"
        "}"
        "QComboBox {"
        "    background-color: #1a3a52;"
        "    color: #e3f2fd;"
        "    border: 2px solid #2e5a7d;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "}"
        "QComboBox:hover {"
        "    border: 2px solid #42a5f5;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "    width: 30px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border-left: 5px solid transparent;"
        "    border-right: 5px solid transparent;"
        "    border-top: 5px solid #90caf9;"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: #132f4c;"
        "    color: #e3f2fd;"
        "    selection-background-color: #1565c0;"
        "    border: 2px solid #1e4976;"
        "}"
        "QLabel {"
        "    color: #90caf9;"
        "    background-color: transparent;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: #0d2137;"
        "    width: 12px;"
        "    border: none;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: #2e5a7d;"
        "    border-radius: 6px;"
        "    min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: #42a5f5;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"
        "}";
    
    this->setStyleSheet(navy_theme);
}

void MainWindow::on_mode_changed(int index) {
    current_mode_ = (index == 0) ? SOCKET : SHARED_MEMORY;
    update_mode_ui();
}

void MainWindow::on_connect_clicked() {
    if (is_connected_) {
        if (current_mode_ == SOCKET && socket_client_) {
            socket_client_->disconnect();
        } else if (current_mode_ == SHARED_MEMORY && shm_client_) {
            shm_client_->leave_room();
        }
    } else {
        QString username = username_input_->text().trimmed();
        if (username.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please enter a username");
            return;
        }

        if (current_mode_ == SOCKET) {
            QString ip = ip_input_->text().trimmed();
            bool ok;
            int port = port_input_->text().toInt(&ok);
            
            if (ip.isEmpty() || !ok || port <= 0 || port > 65535) {
                QMessageBox::warning(this, "Error", "Invalid IP address or port");
                return;
            }

            if (!socket_client_) {
                socket_client_ = std::make_unique<SocketClient>();
                connect(socket_client_.get(), &SocketClient::message_received,
                       this, &MainWindow::on_socket_message_received);
                connect(socket_client_.get(), &SocketClient::connected,
                       this, &MainWindow::on_socket_connected);
                connect(socket_client_.get(), &SocketClient::disconnected,
                       this, &MainWindow::on_socket_disconnected);
                connect(socket_client_.get(), &SocketClient::error_occurred,
                       this, &MainWindow::on_socket_error);
            }

            socket_client_->connect_to_server(ip, port, username);
            
        } else {
            QString shm_name = shm_name_input_->text().trimmed();
            if (shm_name.isEmpty()) {
                QMessageBox::warning(this, "Error", "Please enter shared memory name");
                return;
            }

            if (!shm_client_) {
                shm_client_ = std::make_unique<ShmClient>();
                connect(shm_client_.get(), &ShmClient::message_received,
                       this, &MainWindow::on_shm_message_received);
                connect(shm_client_.get(), &ShmClient::connected,
                       this, &MainWindow::on_shm_connected);
                connect(shm_client_.get(), &ShmClient::disconnected,
                       this, &MainWindow::on_shm_disconnected);
                connect(shm_client_.get(), &ShmClient::error_occurred,
                       this, &MainWindow::on_shm_error);
            }

            shm_client_->join_room(shm_name, username);
        }
    }
}

void MainWindow::on_send_clicked() {
    QString text = message_input_->text().trimmed();
    if (text.isEmpty() || !is_connected_) {
        return;
    }

    // Get current timestamp
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString username = username_input_->text().trimmed();
    
    // Display message locally FIRST (so sender sees it immediately)
    display_message(username, timestamp, text);

    // Then send to server/other clients
    bool success = false;
    if (current_mode_ == SOCKET && socket_client_) {
        success = socket_client_->send_message(text);
    } else if (current_mode_ == SHARED_MEMORY && shm_client_) {
        success = shm_client_->send_message(text);
    }

    if (success) {
        message_input_->clear();
    } else {
        QMessageBox::warning(this, "Error", "Failed to send message");
    }
}

void MainWindow::on_socket_message_received(const QString& username,
                                           const QString& timestamp,
                                           const QString& text) {
    display_message(username, timestamp, text);
}

void MainWindow::on_socket_connected() {
    is_connected_ = true;
    update_connection_ui();
    display_system_message("Connected to server");
}

void MainWindow::on_socket_disconnected() {
    is_connected_ = false;
    update_connection_ui();
    display_system_message("Disconnected from server");
}

void MainWindow::on_socket_error(const QString& error) {
    QMessageBox::critical(this, "Connection Error", error);
    is_connected_ = false;
    update_connection_ui();
}

void MainWindow::on_shm_message_received(const QString& username,
                                        const QString& timestamp,
                                        const QString& text) {
    display_message(username, timestamp, text);
}

void MainWindow::on_shm_connected() {
    is_connected_ = true;
    update_connection_ui();
    display_system_message("Connected to shared memory");
}

void MainWindow::on_shm_disconnected() {
    is_connected_ = false;
    update_connection_ui();
    display_system_message("Disconnected from shared memory");
}

void MainWindow::on_shm_error(const QString& error) {
    QMessageBox::critical(this, "Shared Memory Error", error);
    is_connected_ = false;
    update_connection_ui();
}

void MainWindow::update_connection_ui() {
    if (is_connected_) {
        status_label_->setText("Status: <span style='color: #4caf50;'>Connected</span>");
        connect_button_->setText("Disconnect");
        send_button_->setEnabled(true);
        
        mode_combo_->setEnabled(false);
        ip_input_->setEnabled(false);
        port_input_->setEnabled(false);
        shm_name_input_->setEnabled(false);
        username_input_->setEnabled(false);
    } else {
        status_label_->setText("Status: <span style='color: #f44336;'>Disconnected</span>");
        connect_button_->setText("Connect");
        send_button_->setEnabled(false);
        
        mode_combo_->setEnabled(true);
        username_input_->setEnabled(true);
        update_mode_ui();
    }
}

void MainWindow::update_mode_ui() {
    if (current_mode_ == SOCKET) {
        ip_label_->setVisible(true);
        ip_input_->setVisible(true);
        ip_input_->setEnabled(!is_connected_);
        
        port_label_->setVisible(true);
        port_input_->setVisible(true);
        port_input_->setEnabled(!is_connected_);
        
        shm_name_label_->setVisible(false);
        shm_name_input_->setVisible(false);
    } else {
        ip_label_->setVisible(false);
        ip_input_->setVisible(false);
        
        port_label_->setVisible(false);
        port_input_->setVisible(false);
        
        shm_name_label_->setVisible(true);
        shm_name_input_->setVisible(true);
        shm_name_input_->setEnabled(!is_connected_);
    }
}

void MainWindow::display_message(const QString& username,
                                const QString& timestamp,
                                const QString& text) {
    QString formatted = QString("<span style='color: #64b5f6;'>[%1]</span> "
                               "<span style='color: #81c784; font-weight: bold;'>%2:</span> "
                               "<span style='color: #e3f2fd;'>%3</span>")
                       .arg(timestamp)
                       .arg(username)
                       .arg(text);
    message_display_->append(formatted);
    
    QScrollBar* scrollbar = message_display_->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void MainWindow::display_system_message(const QString& text) {
    QString formatted = QString("<span style='color: #90caf9; font-style: italic;'>*** %1 ***</span>")
                       .arg(text);
    message_display_->append(formatted);
    
    QScrollBar* scrollbar = message_display_->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}
