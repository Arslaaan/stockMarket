#pragma once

#include <chrono>
#include <string>

class Order {
   public:
    Order(const std::string& id_, const std::string& clientId_, size_t amount_,
          double cost, bool isBuy_);
    const std::string& getId() const;
    const std::string& getClientId() const;
    std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;
    size_t getAmount() const;
    void setAmount(size_t newAmount);
    double getCost() const;
    bool isBuyOrder() const;
    bool isActiveOrder() const;
    void deactivate();

   private:
    std::string id;        // уникальный id заявки
    std::string clientId;  // создатель заявки
    std::chrono::time_point<std::chrono::system_clock>
        timestamp;  // время создания
    size_t amount;  // количество валюты
    double cost;    // стоимость единицы валюты
    bool isBuy;  // true - это заявка на покупку, иначе - продажа
    bool isActive;  // true - это активная заявка, иначе - совершенная
};