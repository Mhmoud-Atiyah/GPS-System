cmake_minimum_required(VERSION 3.26)
project(gps)

set(CMAKE_CXX_STANDARD 23)
#add_definitions(-DMCU)


include_directories(include unit net libs libs/TinyGPSPlus/src libs/json/include)
set(COMMON_SRC
        libs/logger.cpp
        libs/TinyGPSPlus/src/TinyGPS++.cpp
        net/mongoose.c
        net/net.cpp
        unit/unit.cpp
        "unit/io.cpp"
        include/packet.h
        net/packet.cpp)
# GPS Server Executable
add_executable(server server.cpp ${COMMON_SRC})
# DEVICE Executable
add_executable(Device MCU.cpp ${COMMON_SRC}
        libs/server.h)

find_package(Boost REQUIRED COMPONENTS system thread)
target_link_libraries(Device PRIVATE Boost::system Boost::thread pthread)
target_link_libraries(server PRIVATE Boost::system Boost::thread pthread)