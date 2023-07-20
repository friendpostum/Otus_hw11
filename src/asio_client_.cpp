#include <boost/asio.hpp>

#include <iostream>

namespace ba = boost::asio;

int main(int, char *[]) {

    std::locale::global(std::locale(""));

    try {
        ba::io_context io_context;

        ba::ip::tcp::endpoint ep(
            ba::ip::address::from_string(
                "127.0.0.1"
            ),
            90
        );
        ba::ip::tcp::socket sock(io_context);

        sock.connect(ep);

        //std::string msg{"INSERT A 0 lean"};
        //std::string msg{"TRUNCATE A"};
        std::string msg{"INTERSECTION"};
       // std::string msg{"SYMMETRIC_DIFFERENCE"};

        ba::write(sock, ba::buffer(msg.c_str(), msg.size()));

        char data[20];
        size_t len = sock.read_some( ba::buffer(data) );
        std::cout << "=" << data << std::endl;
    }
    catch(const boost::system::system_error& ex) {
        std::cout << "boost exception! " << ex.what() << std::endl;
    }
    catch(const std::exception& ex) {
        std::cout << "std::exception! " << ex.what() << std::endl;
    }

    return 0;
}
