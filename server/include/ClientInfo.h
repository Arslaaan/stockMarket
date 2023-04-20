#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "Common.hpp"

// Хранит информацию по определенному клиенту
class ClientInfo {
   public:
    ClientInfo(const std::string& userId_, const std::string& userName_,
               const std::string& auth_);

    /// @brief Добавить на счет income
    /// @param type тип валюты
    /// @param income количество валюты
    void addToBalance(const std::string& type, double income);
    /// @brief Текущий баланс
    /// @param type тип валюты
    double getBalance(const std::string& type) const;
    const std::string& getUserName() const;

    /// @brief Проверить токен, что клиент тот за кого себя выдает
    /// @param auth токен
    /// @return true если успех
    bool checkAuth(const std::string& auth) const;
    /// @brief Снять со счета income
    /// @param type тип валюты
    void takeFromBalance(const std::string& type, double expenditure);
    void historyUpdate(const std::string& tradeId);
    const std::vector<std::string>& getHistory() const;
    const std::vector<std::string>& getActiveOrders() const;
    /// @brief Добавить активную заявку
    void addActiveOrder(const std::string& orderId);
    /// @brief Удалить активную заявку, после чего она становится неактивной
    void removeOrder(const std::string& orderId);

   private:
    std::map<std::string, double>
        balance_;  // Будем считать, что double нам хватает. Тут валютный баланс
                   // может быть отрицательным + для единообразия с рубля double
    std::vector<std::string>
        history_;  // история совершенных сделок клиента. Хранит tradeId. Вектор
                   // для сохранения хронологического порядка
    std::vector<std::string> activeOrders_;  // хранит id активных заявок
    std::string auth;  // используется для аутентификации клиента, личный токен
    std::string userName;  // имя/логин
    std::string userId;    // присвоенный id
};