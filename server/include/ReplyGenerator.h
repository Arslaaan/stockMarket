#pragma once

#include "Core.h"
#include "json.hpp"

// Возвращает ответы клиенту
class ReplyGenerator {
   public:
    ReplyGenerator(Core& core);
    std::string handleRegister(const std::string& userName, const std::string& pass);
    std::string handleLogin(const std::string& userName, const std::string& pass);
    // Покупка или продажа, матчит заявки. Возвращает успешность операции.
    std::string handleTransaction(const std::string& userId, size_t amount,
                                  double cost, const std::string& type);
    // Возвращает баланс клиента.
    std::string handleBalance(const std::string& userId);
    // Возвращает историю совершенных сделок (в том числе частично выполненных).
    std::string handleHistory(const std::string& userId);
    // Возвращает активные сделки
    std::string handleActive(const std::string& userId);
    // Проверка что пользователь зарегистрирован и пришел с правильным хэшем пароля
    std::optional<std::string> checkUser(const std::string& userId, const std::string& auth);

   private:
    Core& core_;
};