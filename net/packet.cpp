#include "packet.h"
#include <algorithm>

int Packet::sliceToInt(const std::vector<uint8_t> &data, int start, int end) {
    int result = 0;
    for (int i = start; i < end; ++i) {
        result = (result << 8) | data[i];
    }
    return result;
}

std::vector<uint8_t> Packet::hexStringToBytes(const std::string &hex) {
    if (hex.length() % 2 != 0) {
        throw std::invalid_argument("Hex string length must be even.");
    }

    std::vector<uint8_t> bytes;
    bytes.reserve(hex.length() / 2);

    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        auto byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

std::string Packet::byteToHex(uint8_t byte) {
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int) byte;
    return oss.str();
}

std::string Packet::hex_to_utf8(const std::vector<uint8_t> &data) {
    return std::string(data.begin(), data.end());
}

std::string Packet::trim(const std::string &input) {
    return input.substr(0, 10);  // Extract the first 10 characters
}

uint8_t Packet::computeChecksum(const std::vector<uint8_t> &data) {
    uint8_t checksum = 0x00;
    for (uint8_t byte: data) {
        checksum ^= byte;
    }
    return checksum;
}

Packet Packet::parsePacket(const std::string &hex_string) {
    Packet packet;
    std::vector<uint8_t> packetHex = hexStringToBytes(hex_string);

    if (packetHex.size() < 11 || hex_string.substr(0, 4) != HEADER) {
        std::cerr << "Invalid or too short packet.\n";
        return packet;
    }

    try {
        packet.type = packetHex.at(2);  // TODO: handle based on type
        packet.command = packetHex.at(3);
        packet.deviceId = sliceToInt(packetHex, 4, 8);
        packet.data_length = (short) sliceToInt(packetHex, 8, 10);
        std::vector<uint8_t> packetHex_data;
        if (packet.data_length > 0) {
            auto start = packetHex.begin();
            start += 10;
            packetHex_data.assign(start, start + packet.data_length);
            packet.data = hex_to_utf8(packetHex_data);
        }
        const uint8_t checksumValue = packetHex.back();
        packetHex.pop_back();
        packet.checksum = computeChecksum(packetHex);
        packet.checksum == checksumValue ? packet.valid = true : packet.valid = false;
    } catch (const std::exception &e) {
        std::cerr << "Error parsing: " << e.what() << "\n";
    }
    return packet;
}

std::string Packet::createPacket() const {
    std::vector<uint8_t> rawBytes;

    rawBytes.push_back(type);
    rawBytes.push_back(command);

    rawBytes.push_back((deviceId >> 24) & 0xFF);
    rawBytes.push_back((deviceId >> 16) & 0xFF);
    rawBytes.push_back((deviceId >> 8) & 0xFF);
    rawBytes.push_back(deviceId & 0xFF);

    rawBytes.push_back((data_length >> 8) & 0xFF);
    rawBytes.push_back(data_length & 0xFF);

    for (char c: data) {
        rawBytes.push_back(static_cast<uint8_t>(c));
    }

    uint8_t checksum = computeChecksum(rawBytes);

    // Build hex string
    std::ostringstream result;
    result << HEADER;
    for (uint8_t byte: rawBytes) {
        result << byteToHex(byte);
    }
    result << byteToHex(checksum);  // append checksum at the end

    return result.str();
}

void Packet::printPacket() const {
    if (!valid) {
        std::cout << "Invalid packet.\n";
//        return;
    }
    std::cout << "Type        : " << static_cast<int>(type) << "\n";
    std::cout << "Command     : " << static_cast<int>(command) << "\n";
    std::cout << "Device ID   : " << deviceId << "\n";
    std::cout << "Data Length : " << data_length << "\n";
    std::cout << "Data        : " << data.data() << "\n";
    std::cout << "Checksum    : " << static_cast<int>(checksum) << "\n";
}
