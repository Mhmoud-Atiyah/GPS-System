#ifndef GPS_SERVER_H
#define GPS_SERVER_H

#include <asio.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "packet.h"
#include "net.h"
#include "logger.h"
#include "nlohmann/json.hpp"

std::unordered_map<std::string, std::string> loadDeviceClientMap(const std::string &filename) {
    std::unordered_map<std::string, std::string> map;
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return map;
    }

    std::string line;
    while (std::getline(infile, line)) {
        size_t delim = line.find('=');
        if (delim != std::string::npos) {
            std::string deviceId = line.substr(0, delim);
            std::string clientId = line.substr(delim + 1);
            map[deviceId] = clientId;
        }
    }

    return map;
}

#endif //GPS_SERVER_H
