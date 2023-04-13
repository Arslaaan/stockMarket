#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "Common.hpp"

// Хранит информацию по клиенту
class ClientInfo {
   public:
    ClientInfo();
    void addToBalance(const std::string& type, double income);
    double getBalance(const std::string& type) const;
    void takeFromBalance(const std::string& type, double expenditure);
    void historyUpdate(const std::string& tradeId);
    const std::vector<std::string>& getHistory() const;
    const std::vector<std::string>& getActiveOrders() const;
    void addActiveOrder(const std::string& orderId);
    void removeOrder(
        const std::string& orderId);  // убрать заявку, ставшую неактивной

   private:
    std::map<std::string, double>
        balance_;  // Будем считать, что double нам хватает. Тут валютный баланс
                   // может быть отрицательным + для единообразия с рубля double
    std::vector<std::string>
        history_;  // история сделок клиента. Хранит tradeId. Вектор для
                   // сохранения хронологического порядка
    std::vector<std::string> activeOrders_;  // хранит id активных заявок
};