#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include <cstdint>
#include <bitset>

#define SET_BIT(flag, bit)       ((flag) |= (1U << (bit)))   // Set bit to 1
#define CLEAR_BIT(flag, bit)     ((flag) &= ~(1U << (bit)))  // Clear bit (set to 0)
#define TOGGLE_BIT(flag, bit)    ((flag) ^= (1U << (bit)))   // Toggle bit state
#define CHECK_BIT(flag, bit)     (((flag) >> (bit)) & 1U)    // Check if bit is set (returns 1 or 0)

// Device State Flags Position (Enable/Disable)
#define PWR_ACC     0   // Example: Ignition(ACC)
#define PWR_COE     1   // Example: Cut-Of-Engine
#define IO_SOS      2   // Example: SOS Button
#define IO_MIC      3   // Example: Microphone Input
#define IO_SPEAKER  4   // Example: Speaker Output
#define IO_DOOR     5   // Example: Door closed ?
#define IO_TEMP     6   // Example: Temperature Sensor
#define IO_FUEL     7   // Example: Fuel Sensor

// Global Device I/O State Flags
static uint8_t DeviceIOStatue;

struct IO_Payload {
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
    IO_Payload(uint32_t
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
