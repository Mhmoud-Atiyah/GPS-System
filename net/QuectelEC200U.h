#ifndef GPS_QUECTELEC200U_H
#define GPS_QUECTELEC200U_H

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include "logger.h"
#include "packet.h"

class QuectelEC200USimulator {
public:
    explicit QuectelEC200USimulator(const std::string &filename)
            : filename(filename) {
        ensurePipeExists();
        file.open(filename);  // Try to open initially
    }

    bool isOpen() const { return file.is_open(); }

    std::string readLine() {
        std::string line;

        if (!file.is_open()) {
            file.open(filename);  // Reopen file
            if (!file) {
                std::this_thread::sleep_for(std::chrono::seconds(PULSE_TIME));
                return "";
            }
        }

        if (std::getline(file, line)) {
            return line;
        } else {
            file.close();  // Rewind every few seconds
            std::this_thread::sleep_for(std::chrono::seconds(PULSE_TIME));
            return "";
        }
    }


private:
    std::string filename;
    std::ifstream file;

    void ensurePipeExists() {
        if (access(filename.c_str(), F_OK) == -1) {
            if (mkfifo(filename.c_str(), 0666) == -1) {
                std::cerr << "Failed to create FIFO: " << strerror(errno) << std::endl;
                std::exit(EXIT_FAILURE);  // Exit if pipe creation fails
            } else {
                std::cout << "FIFO created at " << filename << std::endl;
            }
        } else {
            std::cout << "FIFO already exists at " << filename << std::endl;
        }
    }
};

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

    void simulateIncomingData(QuectelEC200USimulator &modem) {
        std::string line = modem.readLine();
        if (line.empty()) return;
        Logger::log(gps::LogLevel::ALARM, "[Simulated Modem][CMD]", line);
        // Check for incoming data event
        if (line.find("+QIURC: \"recv\"") != std::string::npos) {
            // Read the actual payload from file
            std::string payload = modem.readLine();
            Logger::log(gps::LogLevel::ALARM, "[Simulated Modem][MSG]", payload);
            Packet pkt = Packet::parsePacket(payload);
            pkt.printPacket();
            // Main Routine
            //FIXME: checksum validation
            if (!pkt.valid && pkt.type == 0x02) {
                switch (pkt.command) {
                    case 0x10: // STOP engine
                        Logger::log(gps::LogLevel::INFO, "[Simulated Modem][EXE]", "Engine Stooped Successfully");
                        // do_STOP_ENGINE();
                        break;
                    default:
                        Logger::log(gps::LogLevel::ERROR, "[Simulated Modem][ERR]", "Command Not Recognized");
                        break;
                }
            }
        }
    }

private:
    std::unordered_map<std::string, std::string> responses;
};

#endif //GPS_QUECTELEC200U_H
