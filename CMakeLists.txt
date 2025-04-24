cmake_minimum_required(VERSION 3.26)
project(gps)

set(CMAKE_CXX_STANDARD 23)
#add_definitions(-DMCU)

include_directories(include unit net libs libs/TinyGPSPlus/src)
add_executable(gps
        libs/logger.cpp
        libs/TinyGPSPlus/src/TinyGPS++.cpp
        net/mongoose.c
        net/net.cpp
        unit/unit.cpp
        MCU.cpp
        server.cpp
        "unit/io.cpp"
        include/packet.h
        net/packet.cpp
)
find_package(Boost REQUIRED COMPONENTS system thread)
target_link_libraries(gps PRIVATE Boost::system Boost::thread pthread)