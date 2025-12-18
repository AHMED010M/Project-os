// MIT License
// Multi-threaded Chat System - Client Entry Point
// Copyright (c) 2025

#include "MainWindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("OS Chat Client");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("OS Chat Project");

    // Create and show main window
    MainWindow window;
    window.setWindowTitle("OS Chat Client");
    window.resize(800, 600);
    window.show();

    return app.exec();
}
