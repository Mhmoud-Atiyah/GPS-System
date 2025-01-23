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
        uint8_t command[] = {0xB5, 0x62, 0x01, 0x07, 0x00};  // Example UBX command
        sendCommand(command, sizeof(command));
        std::string gpsData = receiveGPSData();
        if (receivedDataQueue.push(gpsData) == return_error_queue) {
            return;
            //TODO: just sleep thread until unit remove items from queue
        }
        // START_ for testing ==> Log commands
        if (!gpsData.empty()) {
            std::strstream ss;
            ss << "Received GP4S Data: " << gpsData;
            Logger::log(INFO, "GNSS", ss.str());
            std::cout << std::endl;
        }
        // END_ for testing ==> Log commands
    }
}

void GNSS::sendCommand(uint8_t *command, uint8_t length) {
    // Simulate sending a command to the GPS (e.g., to request data or configure the GPS)
    Logger::log(DEBUG, "GNSS", "Sending command to GPS: ");
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

void unit::push(const std::string &server, const std::string &port) {

}
