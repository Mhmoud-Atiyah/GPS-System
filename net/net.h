#ifndef GPS_NET_H
#define GPS_NET_H

#include <cstdint>
#include <logger.h>
#include "mongoose.h"
#include "packet.h"
#include <strstream>
#include <sstream>
#include <utility>

namespace gps {

    uint8_t calculateChecksum(std::basic_string<char, std::char_traits<char>, std::allocator<char>> data, size_t length);

}
#endif //GPS_NET_H
