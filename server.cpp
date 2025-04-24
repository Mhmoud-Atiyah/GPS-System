#include <asio.hpp>
#include <iostream>
#include <memory>
#include <unordered_set>
#include "packet.h"

using asio::ip::tcp;

constexpr int PORT = 8181;

void reqHandle(Packet &pkt);

std::string wrap_message(const std::string &msg) {
    return "<<MSG>>" + msg + "<<END>>";
}

std::string unwrap_message(const std::string &msg) {
    const std::string start = "<<MSG>>";
    const std::string end = "<<END>>";
    auto s = msg.find(start);
    auto e = msg.find(end);
    if (s != std::string::npos && e != std::string::npos && e > s) {
        return msg.substr(s + start.size(), e - (s + start.size()));
    }
    return {};
}

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::unordered_set<std::shared_ptr<Session>> &sessions)
            : socket_(std::move(socket)), all_sessions(sessions) {}

    void start() {
        all_sessions.insert(shared_from_this());
        do_read();
    }

    void send_to(std::string msg) {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(wrap_message(msg)),
                          [this, self](std::error_code ec, std::size_t /*length*/) {
                              if (ec) all_sessions.erase(self);
                          });
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_),
                                [this, self](std::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        std::string received(data_.data(), length);
                                        std::string unwrapped = unwrap_message(received);
                                        std::cout << "Received: " << unwrapped << "\n";
                                        Packet packet = Packet::parsePacket(unwrapped);
                                        reqHandle(packet);
                                        // Print the packet
                                        packet.printPacket();
                                        if (unwrapped == "IAM:CLIENT") {
                                            is_client = true;
                                            std::cout << "Client identified.\n";
                                        } else if (unwrapped == "IAM:DEVICE") {
                                            is_device = true;
                                            std::cout << "Device identified.\n";
                                        } else {
                                            // Route message from client to device or vice versa
                                            for (auto &sess: all_sessions) {
                                                if (sess.get() != this && (
                                                        (is_client && sess->is_device) ||
                                                        (is_device && sess->is_client))) {
                                                    sess->send_to(unwrapped);
                                                }
                                            }
                                        }

                                        do_read();
                                    } else {
                                        all_sessions.erase(self);
                                    }
                                });
    }

    tcp::socket socket_;
    std::array<char, 1024> data_;
    std::unordered_set<std::shared_ptr<Session>> &all_sessions;
    bool is_client = false;
    bool is_device = false;
};

class Server {
public:
    Server(asio::io_context &io_context)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), PORT)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
                [this](std::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<Session>(std::move(socket), sessions)->start();
                    }
                    do_accept();
                });
    }

    tcp::acceptor acceptor_;
    std::unordered_set<std::shared_ptr<Session>> sessions;
};

void reqHandle(Packet &pkt) {
    if (!pkt.valid) {
        if (pkt.type == 1) {
            switch (pkt.command) {
                case 0x01: // Handshake
                    std::cout << "New Connection From Device:" << pkt.data << std::endl;
                    break;
                default:
                    std::cout << "default" << std::endl;
                    break;
            }
        } else if (pkt.type == 2) {
            switch (pkt.command) {
                case 0x01: // Handshake
                    std::cout << "New Connection From Client:" << pkt.data << std::endl;
                    break;
                default:
                    std::cout << "default" << std::endl;
                    break;
            }
        }

    }
};

int main() {
    try {
        asio::io_context io_context;
        Server server(io_context);
        std::cout << "Server running on port " << PORT << "\n";
        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
