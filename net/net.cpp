#include "net.h"

uint8_t gps::calculateChecksum(std::basic_string<char, std::char_traits<char>, std::allocator<char>> data, size_t length) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum ^= data[i];  // XOR the current byte with the checksum
    }
    return checksum;
}