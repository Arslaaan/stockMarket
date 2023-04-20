#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <string>

#include "Common.hpp"
#include "json.hpp"
#include "TimeUtils.h"

using boost::asio::ip::tcp;

class Client {
   public:
    Client(boost::asio::io_service& io_service);
    // Логин на сервер
    void login(const std::string& name, const std::string& pass);
    // Регистрация на сервере
    void registration(const std::string& name, const std::string& pass);
    // Открытие заявки на покупку или продажу
    void openOrder(const std::string& type, const std::string& amount,
                   const std::string& cost);
    // Запрос текущего баланса
    void balance();
    // Запрос текущей котировки
    void quote();
    // Запрос истории совершенных сделок
    void history();
    // Запрос активных заявок
    void active();
    // Отмена активной заявки
    void cancel(const std::string& orderId);

    tcp::socket& getSocket();
    const std::string& getId() const;

   private:
    void SendMessage(const std::string& aRequestType,
                     const std::string& aMessage);
    void SendMessage(const std::string& aRequestType,
                     const std::string& aMessage, const std::string& auth_);

    std::string ReadMessage();

    std::string name;
    std::string auth;
    std::string id = "0";
    tcp::socket aSocket;
};