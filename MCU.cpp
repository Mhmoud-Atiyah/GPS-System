#include <iostream>
#include <unit.h>
#include <net.h>

#define IMEI "864205069515823"
#define ID 0x12345678
#define REMOTE "127.0.0.1"
#define PORT "8181"

int main() {
    boost::asio::io_context io;

    gps::unit U1(IMEI, ID, io, REMOTE, PORT);
    U1.startGNSS();// Start the GNSS in a separate thread
    U1.startNet(); // run communications Module in a separate thread
    if (U1.connect()) {
        Packet pkt;
        pkt(0x01, 0x01, ID, sizeof(IMEI), IMEI);
        std::string createdPacketHex = pkt.createPacket();
        U1.send(wrap_message(createdPacketHex));
        std::string response = U1.receive();
        Logger::log(INFO, "NET", unwrap_message(response));
    }
    U1.Parser();
    while (true) {
        U1.run(U1);
    }
}