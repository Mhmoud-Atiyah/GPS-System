#ifndef GPS_NET_H
#define GPS_NET_H

#define NET_TYPES_SOCKET "00"
#define NET_TYPES_POSITION "80"
#define NET_TYPES_SENSOR "A3"
#define NET_PACKET_DELIMITER_START "2929"
#define NET_PACKET_DELIMITER_END "0D"

#include <cstdint>
#include <logger.h>

#ifdef MCU
#else

#include <strstream>
#include <sstream>
#include <utility>

#endif
namespace gps {
    enum NET_TYPES {
        SOCKET,
        POSITION,
        SENSOR
    };

    uint8_t
    calculateChecksum(std::basic_string<char, std::char_traits<char>, std::allocator<char>> data, size_t length) {
        uint8_t checksum = 0;

        for (size_t i = 0; i < length; ++i) {
            checksum ^= data[i];  // XOR the current byte with the checksum
        }

        return checksum;
    }

    /*
     * EXAMPLE:  Packet packet(1616161616, 0xC0A80001, 0xC0A80002, 0xA3, 12345, 50, 0x01, IMEI);
     * */
    struct Packet {
        // Constructor for easy initialization
        Packet(uint32_t ts_sec, uint32_t src_ip, uint32_t dst_ip, uint8_t proto,
               uint32_t seq_num, uint16_t size, uint8_t flgs, std::string creator)
                : timestamp(ts_sec), source(src_ip), destination(dst_ip),
                  type(proto), sequence_number(seq_num), payload_size(size), flags(flgs), owner(std::move(creator)) {}

        // Print packet information
        void print() const {
            std::cout << "Timestamp: " << timestamp << " seconds\n";
            std::cout << "Source IP: " << (source >> 24 & 0xFF) << "."
                      << (source >> 16 & 0xFF) << "."
                      << (source >> 8 & 0xFF) << "."
                      << (source & 0xFF) << "\n";
            std::cout << "Destination IP: " << (destination >> 24 & 0xFF) << "."
                      << (destination >> 16 & 0xFF) << "."
                      << (destination >> 8 & 0xFF) << "."
                      << (destination & 0xFF) << "\n";
            std::cout << "Protocol: " << (int) type << "\n";
            std::cout << "Sequence Number: " << sequence_number << "\n";
            std::cout << "Payload Size: " << payload_size << " bytes\n";
            std::cout << "Flags: " << (int) flags << "\n";
        }

        std::string _toString() {
#ifdef MCU
#else
            std::strstream ss;
            ss << NET_PACKET_DELIMITER_START;
            ss << getNetType(static_cast<NET_TYPES>(this->type));
            ss << this->owner;
            ss << this->payload;
            ss << calculateChecksum(ss.str(), sizeof(ss));
            ss << NET_PACKET_DELIMITER_END;
            return ss.str();
#endif
        }

    private:
        static std::string getNetType(enum NET_TYPES t) {
            switch (t) {
                case POSITION:
                    return NET_TYPES_POSITION;
                    break;
                case SENSOR:
                    return NET_TYPES_SENSOR;
                    break;
                case SOCKET:
                    return NET_TYPES_SOCKET;
                    break;
                default:
                    return NET_TYPES_POSITION;
            }
        }

        std::string owner;
        uint32_t timestamp;
        uint32_t source;
        uint32_t destination;
        // Protocol: 0 - Reserved, 0x80 - position, 0xA3 - positions and Peripheral
        uint8_t type;
        // Sequence number for packet order tracking (e.g., for UDP/TCP packets)
        uint32_t sequence_number;
        // Payload size (in bytes)
        uint16_t payload_size;
        uint8_t payload[80]; // 80-Bytes(80 char) for GNSS payload
        // Flags for real-time protocols (e.g., for TCP, flags might represent SYN, ACK, etc.)
        uint8_t flags; // Bitmask of flags
    };
}
#endif //GPS_NET_H
