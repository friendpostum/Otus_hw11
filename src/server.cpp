#include <iostream>
#include <boost/asio.hpp>
#include "sqlite.h"

#include <set>

namespace ba = boost::asio;

class session : public std::enable_shared_from_this<session> {
public:
    session(ba::ip::tcp::socket socket, Sqlite &db) :
            socket_(std::move(socket)),
            db(db) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec && length > 0) {
                                        std::istringstream commandSequence;
                                        commandSequence.str(std::string{data, length});

                                        std::string cmd;
                                        std::array<std::string, 4> arr;
                                        for (int i = 0; std::getline(commandSequence, arr[i], ' '); ++i) {
                                            //arr[i] = cmd;
                                        }

                                        std::string answ;
                                        if (auto p_cmd = CMD.find(arr[0]); p_cmd != CMD.end()) {
                                            if (*p_cmd == "INSERT") {
                                                answ = db.insert(arr[1], stoi(arr[2]), arr[3]);
                                                //std::cout << "cmd: " << cmd << std::endl;
                                            }

                                            if (*p_cmd == "TRUNCATE") {
                                                answ = db.truncate(arr[1]);
                                                // std::cout << "cmd: " << cmd << std::endl;
                                            }

                                            if (*p_cmd == "INTERSECTION") {
                                                cmd = "SELECT a.id, a.name, b.name FROM a INNER JOIN b ON b.id = a.id";

                                                answ = db.selectEntities(cmd) + "OK";
                                                //std::cout << answ << std::endl;
                                            }

                                            if (*p_cmd == "SYMMETRIC_DIFFERENCE") {
                                                cmd = "SELECT * FROM ("
                                                      " SELECT a.id as id, a.name as name_a, NULL AS name_b FROM a"
                                                      " UNION"
                                                      " SELECT b.id as id, NULL AS name_a, b.name AS name_b FROM b"
                                                      " )"
                                                      " WHERE id NOT IN("
                                                      " SELECT a.id AS id2 FROM a INTERSECT SELECT b.id AS id2 FROM b"
                                                      " ) ORDER BY id";

                                                answ = db.selectEntities(cmd) + "OK";
                                                //std::cout << answ << std::endl;
                                            }

                                            do_write(answ);
                                        } else
                                            std::cout << "Command not found\n";
                                    }
                                });
    }

    void do_write(const std::string &answer) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(answer.c_str(), answer.size()),
                                 [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec) {
                                         do_read();
                                     }
                                 });
    }

    ba::ip::tcp::socket socket_;
    Sqlite &db;
    enum {
        max_length = 1024
    };
    char data[max_length]{};
    std::set<std::string> CMD{"INSERT", "TRUNCATE", "INTERSECTION", "SYMMETRIC_DIFFERENCE"};
};

class Server {
public:
    Server(const size_t port) :
            acceptor(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)) {
        do_accept();
        io_context.run();
    }

private:
    void do_accept() {
        acceptor.async_accept(
                [this](boost::system::error_code ec, ba::ip::tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<session>(std::move(socket), db)->start();
                    } else
                        std::cerr << "Code Error: " << ec.message() << std::endl;
                    do_accept();
                });
    }

    boost::asio::io_context io_context;
    ba::ip::tcp::acceptor acceptor;
    Sqlite db;
};

