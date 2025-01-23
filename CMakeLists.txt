cmake_minimum_required(VERSION 3.26)
project(gps)

set(CMAKE_CXX_STANDARD 23)
#add_definitions(-DMCU)

include_directories(include unit net libs assets/TinyGPSPlus/src)
add_executable(gps
        libs/logger.cpp
        assets/TinyGPSPlus/src/TinyGPS++.cpp
        net/net.cpp
        unit/unit.cpp
        MCU.cpp
        server.cpp
        client.cpp
        net/net.h
        unit/sensor.cpp
        unit/sensor.h
)
