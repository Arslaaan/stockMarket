#pragma once

#include "Core.h"
#include "json.hpp"

// Возвращает ответы клиенту
class ReplyGenerator {
   public:
    ReplyGenerator(Core& core);
    // Авторизацию пользователя (возможно нового).
    // Возвращает его ID.
    std::string handleConnect(const std::string& userName);
    // Покупка или продажа, матчит заявки. Возвращает успешность операции.
    std::string handleTransaction(const std::string& userId, size_t amount,
                                  double cost, const std::string& type);
    // Возвращает баланс клиента.
    std::string handleBalance(const std::string& userId);
    // Возвращает историю совершенных сделок (в том числе частично выполненных).
    std::string handleHistory(const std::string& userId);
    // Возвращает активные сделки
    std::string handleActive(const std::string& userId);

   private:
    Core& core_;
};