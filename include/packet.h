#ifndef GPS_PACKET_H
#define GPS_PACKET_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <utility>
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#define HEADER "FEEF"

class Packet {
public:
    uint8_t type;         // Device = 1 || Client(through REST) = 2
    uint8_t command;
    int deviceId;
    short data_length;
    std::string data;
    uint8_t checksum;
    bool valid = false;

    Packet() = default;

    void operator()(uint8_t type_, uint8_t command_, int deviceId_, short data_length_, std::string data_) {
        type = type_;
        command = command_;
        deviceId = deviceId_;
        data_length = data_length_;
        data = std::move(data_);
    }

    // Parse a hex string into a Packet object
    static Packet parsePacket(const std::string &hex_string);

    // Convert a Packet object to a hex string
    std::string createPacket() const;

    // Print the Packet details
    static std::vector<uint8_t> hexStringToBytes(const std::string &hex);

    static std::string trim(const std::string &input);

    void printPacket() const;

private:
    // Helper functions for parsing and creating packets
    static int sliceToInt(const std::vector<uint8_t> &data, int start, int end);

    static std::string byteToHex(uint8_t byte);

    static std::string hex_to_utf8(const std::vector<uint8_t> &data);

    static uint8_t computeChecksum(const std::vector<uint8_t> &data);
};

#endif //GPS_PACKET_H
