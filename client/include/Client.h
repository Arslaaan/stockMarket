#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

class Client {
   public:
    Client(boost::asio::io_service& io_service);
    void processRegistration(const std::string& name);
    tcp::socket& getSocket();

   private:
    void SendMessage(const std::string& aRequestType,
                     const std::string& aMessage);

    std::string ReadMessage();
    std::string name;
    std::string id = "0";
    tcp::socket aSocket;
};