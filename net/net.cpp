#include "net.h"

uint8_t
gps::calculateChecksum(std::basic_string<char, std::char_traits<char>, std::allocator<char>> data, size_t length) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum ^= data[i];  // XOR the current byte with the checksum
    }
    return checksum;
}

std::string gps::wrap_message(const std::string &msg) {
    return "<<MSG>>" + msg + "<<END>>";
}

std::string gps::unwrap_message(const std::string &msg) {
    const std::string start = "<<MSG>>";
    const std::string end = "<<END>>";
    auto s = msg.find(start);
    auto e = msg.find(end);
    if (s != std::string::npos && e != std::string::npos && e > s) {
        return msg.substr(s + start.size(), e - (s + start.size()));
    }
    return {};
}
