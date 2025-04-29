#include <unit.h>

using namespace gps;

void GNSS::initialize() {
    // START_ for testing ==> Open the file to simulate GPS data from a device
    gpsDataFile.open("../gps_data.txt");
    if (!gpsDataFile.is_open()) {
        std::cerr << "Failed to open GPS data file." << std::endl;
        return;
    }
    // END_ for testing ==> Open the file to simulate GPS data from a device
    while (true) {
        sendCommand(this->command, sizeof(command));
        std::string gpsData = receiveDeviceData();
//        std::stringstream
        // Add received data to Buffer Queue
        if (receivedDataQueue.push({receivedDataQueue_LOCATION, gpsData.append("\r\n")}) == return_error_queue) {
#ifndef MCU
            gps::Logger::log(ERROR, "GNSS", "Queue is full!");
#endif
            std::this_thread::sleep_for(std::chrono::seconds(2));//TODO: sleep thread until unit remove items from queue
        }
        // Log received data
        if (!gpsData.empty() && receivedDataQueue.size() < QUEUE_SIZE) {
            std::strstream ss;
            ss << "Received GPS Data: " << gpsData;
            Logger::log(DEBUG, "GNSS", ss.str());
        }
        // Send Pulse every 3 seconds
        std::this_thread::sleep_for(std::chrono::seconds(PULSE_TIME));
    }
}

void GNSS::sendCommand(uint8_t *command, uint8_t length) {
    // Simulate sending a command to the GPS (e.g., to request data or configure the GPS)
    std::stringstream ss;
    ss << "Sending command to Module: ";
    for (int i = 0; i < length; ++i) {
        ss << std::hex << (int) command[i] << " ";
    }
    Logger::log(DEBUG, "GNSS", ss.str());
}

std::string GNSS::receiveDeviceData() {
    std::string line;
    if (std::getline(gpsDataFile, line)) {
        return line;  // Return a line from the file (simulated GPS data)
    }
    return "";  // Return empty if no data is left
}

void unit::parse(const char *str) {
    std::stringstream ss;
    const char *stream = str;
    while (*stream) {
        if (parser.encode(*stream++)) {
            ss << "[Location]: ";
            if (parser.location.isValid()) {
                ss << parser.location.lat() << "," << parser.location.lng() << "\t";
            } else {
                ss << "INVALID" << std::endl;
            }
            ss << "[Date/Time]: ";
            if (parser.date.isValid()) {
                ss << parser.date.month() << "/" << parser.date.day() << "/" << parser.date.year();
            } else {
                ss << "INVALID" << std::endl;
            }
            ss << "\t";
            if (parser.time.isValid()) {
                if (parser.time.hour() < 10) ss << "0";
                ss << parser.time.hour() << ":";
                if (parser.time.minute() < 10) ss << "0";
                ss << parser.time.minute();
                ss << ":";
                if (parser.time.second() < 10) ss << "0";
                ss << parser.time.second();
                ss << ".";
                if (parser.time.centisecond() < 10) ss << "0";
                ss << parser.time.centisecond();
            } else {
                ss << "INVALID" << std::endl;
            }
            Logger::log(DEBUG, "Parser", ss.str());
        }
    }
}

// MAIN Function
void unit::run(gps::unit &U) {
    while (!receivedDataQueue.empty()) {
        const auto receivedData = receivedDataQueue.frontElement();
        const std::string &type = receivedData.at(0);
        const std::string &data = receivedData.at(1);
        U.parse(data.c_str());// Optional (Emulation)
        // Handle Command
        uint8_t CMD = 0x0;
        if (type == std::string(receivedDataQueue_HANDSHAKE)) {
            CMD = 0x01;
        } else if (type == std::string(receivedDataQueue_LOCATION)) {
            CMD = 0x02;
        }
        pkt(0x01, CMD, ID, data.size(), data);
        std::string createdPacketHex = pkt.createPacket();
        this->send(wrap_message(createdPacketHex));
        std::string response = this->receive();
        Logger::log(INFO, "NET", unwrap_message(response));
        receivedDataQueue.pop();
    };
}

void unit::initializeNET() {
    Logger::log(WARNING, "NET", "Starting Quectel EC200U Module in Simulation Mode");
    QuectelEC200U modem;
    QuectelEC200USimulator modemSim("../EC200U_pipe");

    if (!modemSim.isOpen()) {
        Logger::log(ERROR, "NET", "Error opening FIFO.");
        return;
    }
    // Example AT command to simulate initialization (Module Test)
    std::stringstream ss;
    ss << "[Sending]: " << "AT" << "\t|\t";
    ss << "[Response]: " << modem.sendCommand("AT");
    Logger::log(DEBUG, "NET", ss.str());
    // simulate Live Connection
    while (true)
        modem.simulateIncomingData(modemSim);
}

bool unit::connect() {
    std::stringstream ss;
    ss << "Creating Connection With GPS Server:\t" << host_ << "::" << port_ << "\r";
    Logger::log(WARNING, "NET", ss.str());
    ss.str("");
    try {
        auto endpoints = resolver_.resolve(host_, port_);
        boost::asio::connect(socket_, endpoints);
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Connect error: " << e.what() << std::endl;
        return false;
    }
}

void unit::send(const std::string &message) {
    boost::asio::write(socket_, boost::asio::buffer(message));
}

std::string unit::receive() {
    char reply[1024];
    size_t length = socket_.read_some(boost::asio::buffer(reply));
    return std::string(reply, length);
}