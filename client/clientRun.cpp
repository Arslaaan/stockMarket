#include <boost/asio.hpp>
#include <iostream>

#include "CLI.h"

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1",
                                   std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);
        
        auto client = std::make_unique<Client>(io_service);
        client->getSocket().connect(*iterator);

        CLI cli(client);
        
        cli.ShowHelp();
        std::cout << "Now you can enter your commands:" << std::endl;

        while (true) {
            std::string cmd;
            getline(std::cin, cmd);
            cli.Run(cmd);
        }

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}