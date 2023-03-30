#include "BackendServer.h"
#include "Core.h"

int main() {
    try {
        boost::asio::io_service io_service;
        static Core core;

        Server s(io_service);

        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}