#include "server.h"

using json = nlohmann::json;
using asio::ip::tcp;
using namespace gps;

constexpr int PORT = 8181;

// Global session maps
std::unordered_map<uint32_t, std::shared_ptr<class Session>> device_sessions;
std::unordered_map<std::string, std::shared_ptr<class Session>> client_sessions;
// Retrieve from DB
const auto client_device = loadDeviceClientMap("../client_device_mapping.txt");

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::unordered_set<std::shared_ptr<Session>> &sessions)
            : socket_(std::move(socket)), all_sessions(sessions) {}

    void start() {
        all_sessions.insert(shared_from_this());
        do_read();
    }

    void send_to(const std::string &msg) {
        auto self = shared_from_this();
        asio::async_write(socket_, asio::buffer(wrap_message(msg)),
                          [this, self](std::error_code ec, std::size_t) {
                              if (ec) all_sessions.erase(self);
                          });
    }

private:
    void reqHandle(Packet &pkt, const std::string &pktRaw) {
        auto self = shared_from_this();
        //FIXME: Validation
        if (pkt.valid) return;
        // Device
        if (pkt.type == 1) {
            switch (pkt.command) {
                case 0x01: // Handshake
                    Logger::logServer(INFO, "New Connection From Device", std::to_string(pkt.deviceId));
                    device_sessions[pkt.deviceId] = self;
                    do_write("Hello Device, welcome!");
                    break;

                case 0x02: // Location Update
                    Logger::logServer(INFO, "Location Data", pkt.data);
                    try {
                        const auto clientID = client_device.at(std::to_string(pkt.deviceId));
                        if (client_sessions.contains(clientID)) {
                            client_sessions[clientID]->do_write(pktRaw);
                        }
                    } catch (const std::exception &e) {
                        std::cerr << "Exception: " << e.what() << "\n";
                    }
                    do_write("Ack Location Data");
                    break;

                default:
                    std::cout << "Unhandled device command\n";
                    break;
            }
        }
            // Client
        else if (pkt.type == 2) {
            switch (pkt.command) {
                case 0x01: // Handshake
                    Logger::logServer(INFO, "New Connection From Client", Packet::trim(pkt.data));
                    client_sessions[Packet::trim(pkt.data)] = self;
                    do_write(pktRaw);
                    break;

                default:
                    std::cout << "Unhandled client command\n";
                    break;
            }
        }
    }

    void do_write(const std::string &message) {
        auto self = shared_from_this();
        std::string wrapped = wrap_message(message);

        asio::async_write(socket_, asio::buffer(wrapped),
                          [this, self](std::error_code ec, std::size_t) {
                              if (ec) {
                                  std::cerr << "Write failed: " << ec.message() << "\n";

                                  // Cleanup from device_sessions and client_sessions
                                  for (auto it = device_sessions.begin(); it != device_sessions.end();) {
                                      if (it->second == self) it = device_sessions.erase(it);
                                      else ++it;
                                  }
                                  for (auto it = client_sessions.begin(); it != client_sessions.end();) {
                                      if (it->second == self) it = client_sessions.erase(it);
                                      else ++it;
                                  }

                                  all_sessions.erase(self);
                              }
                          });
    }

    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(asio::buffer(data_),
                                [this, self](std::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        std::string received(data_.data(), length);
                                        std::string unwrapped = unwrap_message(received);
                                        Packet packet = Packet::parsePacket(unwrapped);
                                        Logger::log(ALARM, "Received", unwrapped);
                                        reqHandle(packet, unwrapped);
                                        do_read();
                                    } else {
                                        all_sessions.erase(self);
                                    }
                                });
    }

    tcp::socket socket_;
    std::array<char, 1024> data_;
    std::unordered_set<std::shared_ptr<Session>> &all_sessions;
};

class Server {
public:
    Server(asio::io_context &io_context)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), PORT)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket), sessions_)->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    std::unordered_set<std::shared_ptr<Session>> sessions_;
};

int main() {
    
    try {
        asio::io_context io_context;
        Server server(io_context);
        std::cout << "GPS Server running on port " << PORT << "\n";
        io_context.run();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}