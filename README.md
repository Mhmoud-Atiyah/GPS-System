# 🚀 GPS Communication System

A modular system for bidirectional communication between GPS-enabled devices (e.g. embedded systems like STM32F103) and
user clients via a unified server. The server handles:

- 🌐 HTTP interface for web users
- 🔌 WebSocket interface for live interaction with devices
- 🧭 TCP server to receive device data

---

## 🔧 Features

- **Custom packet format**: Header, command, data, checksum — designed for compact and reliable communication
- **Multi-protocol server**:
    - TCP server for GPS hardware (port `8181`)
    - WebSocket server for control/data sync with devices
    - HTTP server for browser users
- **C++ Client**: Connects to the server using Boost.Asio
- **Web UI**: Real-time device dashboard built with HTML & JavaScript
- **Cross-platform**: Supports Linux server builds, and embedded builds for STM32 via macro `__STM_32__`

---

## 📦 Project Structure

- gps/
- ├── CMakeLists.txt
- ├── MCU.cpp `Entry point for PC Emulation`
- ├── server.cpp `Unified server entry (HTTP + WebSockets + TCP)`
- ├── Emulator.py `Script that emulate gps module via PC port`
- ├── MCU/Core/Src
- │ └── main.c `Entry point for STM32 builds`
- ├── include/
- │ ├── packet.h `Packet format definition`
- │ ├── mongoose.h `Embedded TCP/IP network stack`
- ├── libs/
- │ ├── logger.cpp `Simple Logging library`
- │ └── TinyGPSPlus/ `GPS parsing library`
- ├── net/
- │ ├── net.cpp `Boost WebSocket + TCP server`
- │ ├── mongoose.c `HTTP server for web interface`
- │ ├── packet.cpp `Packet parser and serializer`
- ├── unit/
- │ ├── io.cpp `senors and IO Functions`
- │ └── unit.cpp `main Functions of Devices`
- ├──web/
- │ └──index.html `Web dashboard for live control`

---

## 🛠️ Build Instructions

### Prerequisites

- CMake >= 3.26
- Boost Libraries
- GCC or Clang (Linux)
- STM32 toolchain (for embedded builds)

### Build on Linux

```bash
mkdir build && cd build
cmake .. -DBUILD_FOR_STM32=OFF
make
./gps

cmake .. -DBUILD_FOR_STM32=ON -DCMAKE_TOOLCHAIN_FILE=your_stm32_toolchain.cmake
make
```

### 🖥️ Web Interface

- Open http://localhost:8000 to access the web UI.
- Devices send their location/data over TCP or WebSocket.
- Users can send control commands back to devices via the browser.

### 🌍 Communication

- `Devices → Server: location, speed, sensor data`
- `Server → Web UI: updates dashboard in real-time`
- `User → Server → Devices: control commands`

### 🧪 Packet Format

<table>
  <tr><th>Section</th><th>Length</th><th>Value</th></tr>
  <tr><td>Header</td><td>2 bytes</td><td>0xFEEF</td></tr>
  <tr><td>Type</td><td>1 byte</td><td>0x1: Device | 0x2: Client</td></tr>
  <tr><td>Command</td><td>1 byte</td><td>0x00 :: 0xFF</td></tr>
  <tr><td>Device ID</td><td>4 bytes</td><td>like "12345678"</td></tr>
  <tr><td>Length</td><td>2 bytes</td><td>Data length in bytes</td></tr>
  <tr><td>Data</td><td>N bytes</td><td>string of JSON data "{ }"</td></tr>
  <tr><td>Checksum</td><td>1 byte</td><td>XOR of all previous bytes</td></tr>
</table>

## 📜 License

MIT License — Feel free to use and modify!

## ✨ Credits

Built using C++, Boost, and WebSockets.