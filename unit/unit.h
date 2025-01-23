//
// Created by mhmoud on 1/15/25.
//

#ifndef GPS_UNIT_H
#define GPS_UNIT_H

#define GPS_I2C_ADDR 0x42 // Default I2C address for the SAM-M8Q module
#define QUEUE_SIZE 10 // default queue size

#include <iostream>
#include <fstream>
#include <strstream>
#include <logger.h>
#include <queue.h>
#include <cstdint>

using namespace gps;

namespace gps {
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
        Queue<std::string, QUEUE_SIZE> receivedDataQueue;
    };

    /*
     * main tracker class
     * */
    class unit {
    public:
        explicit unit(const char *unitId) : ID(unitId) {
            GNSS gnss(this->ID);
            // open GNSS Listener
            gnss.initialize();
        };

        void push(const std::string &server, const std::string &port);

    private:
        const char *ID;// IMEI of Device
    };
}
#endif //GPS_UNIT_H
