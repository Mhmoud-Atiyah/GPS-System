#ifndef GPS_QUECTELEC200U_H
#define GPS_QUECTELEC200U_H

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>

class QuectelEC200U {
public:
    QuectelEC200U() {
        responses["AT"] = "OK";
        responses["AT+CGMI"] = "Quectel";
        responses["AT+CGMM"] = "EC200U";
        responses["AT+CGSN"] = "123456789012345"; // Simulated IMEI
        responses["AT+COPS?"] = "+COPS: 0,0,\"Simulated Network\",7";
        responses["AT+CSQ"] = "+CSQ: 25,99"; // Simulated signal quality
        responses["AT+CREG?"] = "+CREG: 0,1"; // Registered to network
    }

    std::string sendCommand(const std::string &command) {
        if (responses.find(command) != responses.end()) {
            return responses[command];
        }
        return "ERROR"; // Default response for unknown commands
    }

private:
    std::unordered_map<std::string, std::string> responses;
};

#endif //GPS_QUECTELEC200U_H
