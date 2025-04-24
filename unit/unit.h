#ifndef GPS_UNIT_H
#define GPS_UNIT_H

#define GPS_I2C_ADDR 0x42 // Default I2C address for the SAM-M8Q module
#define QUEUE_SIZE 10 // default queue size
#define PULSE_TIME 3 // default pulse wait
#define PARSER_Wait 2 // default PARSER wait

#include <iostream>
#include <fstream>
#include <strstream>
#include <logger.h>
#include <queue.h>
#include <cstdint>
#include <TinyGPS++.h>
#include "QuectelEC200U.h"
#include <net.h>

#ifdef MCU
//TODO: Same Solution
#else

#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#endif
using namespace gps;

namespace gps {
    static Queue<std::string, QUEUE_SIZE> receivedDataQueue;
    static Queue<uint32_t, QUEUE_SIZE> receivedIOQueue;

    /*
     * @NOTE: Connect MKR GPS Shield module based on (u-blox SAM-M8Q GNSS)
     *          and STM32F10x as MCU using I2C Interface
     * */
    class GNSS {
    public:
        explicit GNSS(const char *id) : ID(id) {}

        //TODO: Initialize USART Serial Port on MCU
        void initialize();

    private:

        void sendCommand(uint8_t *command, uint8_t length);

        std::string receiveGPSData();

        //TODO: delete after testing
        std::ifstream gpsDataFile;  // File to simulate GPS data input
        const std::string ID;// IMEI
        uint8_t command[5] = {0xB5, 0x62, 0x01, 0x07, 0x00};  // Example UBX command
    };

    /*
     * main tracker class
     * */
    class unit {
    public:
        explicit unit(const char *unitId, boost::asio::io_context &io, const std::string &host, const std::string &port)
                : ID(unitId), socket_(io), resolver_(io), host_(host), port_(port) {
            std::stringstream ss;
            ss << "Creating unit with IMEI: " << ID << "\n";
            Logger::log(LogLevel::WARNING, "Unit", ss.str());
            //TODO: Check H.W ==> connections
        };

        ~unit() {
            if (gnssThread.joinable()) {
                gnssThread.join();
            }
        }

        void Parser() {
            Logger::log(LogLevel::WARNING, "Parser", "Starting Parsing Service\n");
        }

        void parse();

        void startGNSS() {
            gnssThread = std::thread(&unit::initializeGNSS, this);
        }

        void startNet();

        bool connect();

        void start(gps::unit &cp);

        void send(const std::string &message);

        std::string receive();


    private:
        const char *ID;// IMEI of Device
        std::thread gnssThread;    // Thread for GNSS initialization
        TinyGPSPlus parser; // NMEA stream Parser
        tcp::socket socket_;
        tcp::resolver resolver_;
        std::string host_;
        std::string port_;

        void initializeGNSS() {
            Logger::log(WARNING, "GNSS", "Starting GNSS Quectel LC76G Module in Simulation Mode\n");
            GNSS gnss(this->ID);
            // open GNSS Listener
            gnss.initialize();
        }
    };
}
#endif //GPS_UNIT_H
