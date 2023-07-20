#include <iostream>
#include "server.cpp"

int main(int argc, char *argv[]) {
    std::locale::global(std::locale(""));
    if (argc < 2) {
        std::cout << "Count of parameters is incorrect! Please enter sever Port" << std::endl;
        return 1;
    }
    const size_t port = std::stoi(argv[1]);

    Server server(port);

    return 0;
}