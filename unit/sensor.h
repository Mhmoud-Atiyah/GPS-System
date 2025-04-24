#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include <cstdint>

struct IO {
    uint32_t deviceId;

    // Timestamp of when the data was recorded
    uint64_t timestamp;  // Unix timestamp (milliseconds or seconds)

    // Temperature in Celsius
    float temperature;

    // Humidity in percentage
    float humidity;

    // Pressure in Pascals (for pressure sensors)
    float pressure;

    // Voltage or battery level (if applicable)
    float voltage;

    // Additional sensor status or flags (e.g., if the sensor is calibrated)
    uint8_t statusFlags;

    // Constructor to initialize the sensor data
    IO(uint32_t
       id,
       uint64_t ts,
       float temp,
       float hum,
       float press,
       float volt, uint8_t
       status)
            :

            deviceId(id), timestamp(ts), temperature(temp), humidity(hum), pressure(press), voltage(volt),
            statusFlags(status) {}
};


#endif //GPS_SENSOR_H
