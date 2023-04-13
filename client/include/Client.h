#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>

#include "Common.hpp"
#include "json.hpp"
#include <ctime>

using boost::asio::ip::tcp;

class Client {
   public:
    Client(boost::asio::io_service& io_service);
    // Авторизация или регистрация на сервере
    void connect(const std::string& name);
    // Открытие заявки на покупку или продажу
    void openOrder(const std::string& type, const std::string& amount,
                     const std::string& cost);
    // Запрос текущего баланса
    void balance();
    // Запрос истории совершенных сделок
    void history();
    // Запрос активных заявок
    void active();

    
    tcp::socket& getSocket();
    const std::string& getId() const;

   private:
    void SendMessage(const std::string& aRequestType,
                     const std::string& aMessage);

    std::string ReadMessage();

    std::string name;
    std::string id = "0";
    tcp::socket aSocket;
};