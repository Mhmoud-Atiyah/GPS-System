#include <iostream>
#include <unit.h>
#include <net.h>

#define IMEI "864205069515823"
#define ID 0x12345678
#define REMOTE "127.0.0.1"
#define PORT "8181"

int main() {
    boost::asio::io_context io;

    gps::unit U1(IMEI, io, REMOTE, PORT);
    U1.startGNSS();// Start the GNSS in a separate thread
    U1.startNet(); // start communications Module
    if (U1.connect()) {
        std::stringstream dataToSend;
        std::string data = "Hello from Device";
        Packet pkt(0x01, 0x01, ID, data.size(), data);
        std::string createdPacketHex = pkt.createPacket();
        dataToSend << "<<MSG>>" << createdPacketHex << "<<END>>";
        U1.send(dataToSend.str());
        std::string response = U1.receive();
        std::cout << "Response: " << response << std::endl;
    }
    U1.Parser();// (Optional Service) Log to Screen
    // Main Loop
    while (true) {
        U1.start(U1);
    }
}