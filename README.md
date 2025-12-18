# OS Chat Project

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Qt](https://img.shields.io/badge/Qt-5-green.svg)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-red.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

A high-performance, multi-threaded chat system implemented in C++ featuring both network (TCP sockets) and local (shared memory) communication modes with a modern Qt5 GUI.

## 🌟 Features

### Network Communication
- **Multi-threaded TCP Server**: Handles multiple concurrent clients
- **Thread-safe Broadcasting**: Messages sent to all clients except sender
- **Graceful Shutdown**: Proper resource cleanup on SIGINT
- **Connection Management**: Automatic disconnect detection

### GUI Client
- **Dual Mode Support**: Switch between Socket and Shared Memory
- **Dark Theme Interface**: Modern, eye-friendly design
- **Real-time Updates**: Instant message display
- **Connection Status**: Visual feedback for connection state
- **Auto-scroll**: Always see latest messages

### Technical Excellence
- **Thread Safety**: Mutex-protected shared resources
- **Non-blocking GUI**: Separate threads for network operations
- **Error Handling**: Comprehensive error checking
- **Clean Architecture**: Separation of concerns
- **Modern C++17**: Using latest standards

## 📁 Project Structure

```
os-chat-project/
├── CMakeLists.txt           # Root build configuration
├── README.md                # This file
├── shared/                  # Common code
│   ├── protocol.h          # Message protocol definition
│   ├── common.h            # Utility functions header
│   └── common.cpp          # Utility functions implementation
├── server/                  # TCP Server
│   ├── CMakeLists.txt
│   ├── main.cpp            # Server entry point
│   ├── server.h
│   ├── server.cpp          # Server implementation
│   ├── client_handler.h
│   └── client_handler.cpp  # Per-client thread handler
├── client_gui/              # Qt5 GUI Client
│   ├── CMakeLists.txt
│   ├── main.cpp            # Client entry point
│   ├── MainWindow.h
│   ├── MainWindow.cpp      # Main GUI window
│   ├── SocketClient.h
│   ├── SocketClient.cpp    # TCP socket client
│   ├── ShmClient.h
│   └── ShmClient.cpp       # Shared memory client
└── tests/                   # Unit tests
    ├── CMakeLists.txt
    └── basic_test.cpp      # Basic functionality tests
```

## 🔧 Prerequisites

### Required Software
- **CMake** 3.16 or higher
- **C++ Compiler** with C++17 support (GCC 7+, Clang 5+, MSVC 2017+)
- **Qt5** development libraries (Core, Gui, Widgets, Network)
- **pthread** library (usually included on Linux/macOS)

### Ubuntu/Debian Installation
```bash
sudo apt update
sudo apt install -y build-essential cmake qtbase5-dev qtchooser \
    qt5-qmake qtbase5-dev-tools libqt5network5
```

### Fedora/RHEL Installation
```bash
sudo dnf install -y cmake gcc-c++ qt5-qtbase-devel qt5-qtbase-gui
```

### macOS Installation
```bash
brew install cmake qt@5
export CMAKE_PREFIX_PATH="/usr/local/opt/qt@5"
```

## 🚀 Building the Project

### Quick Build
```bash
# Clone the repository
git clone https://github.com/AHMED010M/Project-os.git
cd Project-os

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)
```

### Build Options
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Specify Qt5 location (if needed)
cmake -DCMAKE_PREFIX_PATH=/path/to/qt5 ..
```

## 📖 Usage

### Starting the Server
```bash
cd build
./server/chat_server
```

The server will start listening on `0.0.0.0:5000` by default.

**Server Output:**
```
[INFO] Chat server starting on 0.0.0.0:5000...
[INFO] Server is running. Press Ctrl+C to stop.
[INFO] Client connected: ID 1
[INFO] Client 1 username: Ahmed
[INFO] Broadcasting message from Ahmed
```

### Starting the Client
```bash
cd build
./client_gui/chat_client
```

**Client Steps:**
1. Select mode: **Socket** or **Shared Memory**
2. Enter connection details:
   - Socket: IP address (e.g., `127.0.0.1`) and port (e.g., `5000`)
   - SHM: Shared memory name (e.g., `chat_shm`)
3. Enter your username
4. Click **Connect**
5. Start chatting!

### Keyboard Shortcuts
- **Enter** in message box: Send message
- **Ctrl+C** in server terminal: Graceful shutdown

## 🎨 GUI Features

### Dark Theme
- Background: `#1e1e1e`
- Text: `#e0e0e0`
- Monospace font: 11pt
- Comfortable for extended use

### Message Display Format
```
[2025-12-18T11:55:30Z] Ahmed: Hello everyone!
[2025-12-18T11:55:45Z] Sara: Hi Ahmed!
```

## 🧪 Testing

### Run Basic Tests
```bash
cd build
./tests/basic_tests
```

### Manual Testing Scenarios

1. **Multiple Clients**
   - Start server
   - Launch 3+ client instances
   - Send messages from different clients
      - ✅ Each client should see messages from others
      - ✅ Sender should NOT see their own message duplicated

2. **Disconnect Handling**
   - Connect a client
   - Click "Disconnect" button
   - ✅ GUI should remain responsive
   - ✅ Status should show "Disconnected"
   - ✅ Server should log disconnection

3. **Server Shutdown**
   - Connect multiple clients
   - Press Ctrl+C on server
   - ✅ Server should close all connections
   - ✅ Clients should detect disconnect

## 🔍 Protocol Specification

### Message Structure
```cpp
struct Message {
    char username[MAX_USERNAME_LEN];    // 32 bytes
    char timestamp[MAX_TIMESTAMP_LEN];  // 32 bytes
    char text[MAX_MESSAGE_LEN];         // 512 bytes
};
```

### Connection Flow
1. Client connects to server
2. Client sends username as first message
3. Server acknowledges and adds to client list
4. Client can now send chat messages
5. Server broadcasts each message to all other clients

### Network Byte Order
- All multi-byte integers converted using `htonl()`/`ntohl()`
- Ensures cross-platform compatibility

## ⚠️ Common Issues & Solutions

### Build Errors

**Qt5 not found**
```bash
# Set Qt5 path explicitly
export CMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt5
# Or
cmake -DCMAKE_PREFIX_PATH=/path/to/qt5 ..
```

**C++17 not supported**
```bash
# Update your compiler
sudo apt install g++-9  # Ubuntu
# Then
export CXX=g++-9
```

### Runtime Errors

**Server: "Address already in use"**
```bash
# Port 5000 is occupied, kill existing process
lsof -ti:5000 | xargs kill -9
# Or wait 1-2 minutes for OS to release the port
```

**Client: "Connection refused"**
- Ensure server is running
- Check firewall settings
- Verify IP address and port

**GUI freezes on disconnect**
- This should NOT happen with proper implementation
- Check that `receive_thread_.detach()` is called
- Verify `shutdown()` is called before `close()`

## 🛡️ Security Considerations

⚠️ **This is an educational project** - NOT production-ready!

### Current Limitations
- No encryption (plaintext communication)
- No authentication
- No input validation
- No rate limiting
- No message size limits enforced

### For Production Use, Add:
- TLS/SSL encryption
- User authentication (OAuth, JWT)
- Input sanitization
- Message queue limits
- DoS protection
- Logging and monitoring

## 📚 Learning Resources

### Understanding the Code
- **Multithreading**: Each client runs in its own thread
- **Mutexes**: Protect shared client list from race conditions
- **Sockets**: TCP for reliable, ordered message delivery
- **Qt Signals/Slots**: Event-driven GUI updates
- **RAII**: Automatic resource cleanup

### Key Concepts
1. **Thread Safety**: Why we use `std::mutex`
2. **Non-blocking I/O**: Why `detach()` instead of `join()`
3. **Signal Handling**: Graceful shutdown with SIGINT
4. **Qt Meta-Object System**: How MOC enables signals/slots

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Code Style
- Follow existing code style
- Use meaningful variable names
- Add comments for complex logic
- Keep functions focused and small

## 📄 License

This project is licensed under the MIT License - see below:

```
MIT License

Copyright (c) 2025 OS Chat Project Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 Acknowledgments

- Qt Framework for the excellent GUI toolkit
- C++ Standard Committee for C++17
- POSIX for socket and threading APIs
- All contributors and testers

## 📧 Contact

For questions, issues, or suggestions:
- Open an issue on GitHub
- Check existing issues first

---

**Built with ❤️ using C++17 and Qt5**
