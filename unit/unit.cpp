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
        std::string gpsData = receiveGPSData();
        if (receivedDataQueue.push(gpsData.append("\r\n")) == return_error_queue) {
#ifndef MCU
            gps::Logger::log(ERROR, "GNSS", "Queue is full!\n");
#endif
            std::this_thread::sleep_for(std::chrono::seconds(2));//TODO: sleep thread until unit remove items from queue
        }
        //TODO: START_ for testing ==> Log commands
        if (!gpsData.empty() && receivedDataQueue.size() < QUEUE_SIZE) {
            std::strstream ss;
            ss << "Received GPS Data: " << gpsData;
            Logger::log(DEBUG, "GNSS", ss.str());
        }
        //TODO: END_ for testing ==> Log commands

        // Send Pulse every 3 seconds
        std::this_thread::sleep_for(std::chrono::seconds(PULSE_TIME));
    }
}

void GNSS::sendCommand(uint8_t *command, uint8_t length) {
    // Simulate sending a command to the GPS (e.g., to request data or configure the GPS)
    Logger::log(DEBUG, "\nGNSS", "Sending command to Module: ");
    for (int i = 0; i < length; ++i) {
        std::cout << std::hex << (int) command[i] << " ";
    }
    std::cout << std::endl;
    //TODO: real implement
    // HAL_I2C_Master_Transmit(&hi2c1, GPS_I2C_ADDR, command, length, HAL_MAX_DELAY);
}

std::string GNSS::receiveGPSData() {
    /*TODO: get char* or NULL from I2c
    * if (HAL_I2C_Master_Receive(&hi2c1, GPS_I2C_ADDR, buffer, length, HAL_MAX_DELAY) != HAL_OK) {
       printf("Error receiving data from GPS\n");
   }*/
    std::string line;
    if (std::getline(gpsDataFile, line)) {
        return line;  // Return a line from the file (simulated GPS data)
    }
    return "";  // Return empty if no data is left
}

void unit::parse() {
    while (!receivedDataQueue.empty()) {
        std::stringstream ss;
        const std::string it = receivedDataQueue.frontElement();
        const char *stream = it.c_str();
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
                ss << std::endl;
                Logger::log(INFO, "Parser", ss.str());
            }
        }
        receivedDataQueue.pop();
    };
}

/*
    MAIN Function
*/
void unit::start(gps::unit &U) {
    // first collect all data from (GNSS, IO)
    U.parse();
}

void unit::startNet() {
    Logger::log(WARNING, "NET", "Starting Quectel EC200U Module in Simulation Mode\n");
    QuectelEC200U modem;
    // Example AT commands to simulate initialization (Module INFO)
    std::string commands[] = {"AT+CGMI", "AT+CGMM", "AT+CGSN", "AT+COPS?", "AT+CSQ", "AT+CREG?"};
    std::stringstream ss;
    for (const auto &cmd: commands) {
        ss << "[Sending]: " << cmd << "\t|\t";
        ss << "[Response]: " << modem.sendCommand(cmd) << "\n";
        Logger::log(DEBUG, "NET", ss.str());
        ss.str("");
    }
    //TODO: Simulate reciever (recieve signal every 3 seconds)
}

bool unit::connect() {
    std::stringstream ss;
    ss << "Creating Connection With GPS Server:\t" << host_ << "::" << port_ << "\n\r";
    Logger::log(WARNING, "NET", ss.str());
    try {
        auto endpoints = resolver_.resolve(host_, port_);
        boost::asio::connect(socket_, endpoints);
        std::cout << "Connected to " << host_ << ":" << port_ << std::endl;
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