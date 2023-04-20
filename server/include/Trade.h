#pragma once

#include <chrono>
#include <iostream>

// Cделка
class Trade {
   public:
    Trade(const std::string& uuidSrc, const std::string& uuidDst,
          size_t amountTraded);
    const std::string& getSrcOrder() const;
    const std::string& getDstOrder() const;
    size_t getAmountTraded() const;
    /// @return Время совершения сделки
    std::chrono::time_point<std::chrono::system_clock> getTradeTime() const;

   private:
    std::string uuidSrc_;  // orderId продающего
    std::string uuidDst_;  // orderId покупающего
    size_t amountTraded_;  // сколько валюты перетекло в результате сделки
    std::chrono::time_point<std::chrono::system_clock>
        tradeTime;  // время совершения сделки
};