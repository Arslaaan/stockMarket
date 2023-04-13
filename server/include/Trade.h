#pragma once

#include <ctime>
#include <iostream>

// Cделка
class Trade {
   public:
    Trade(const std::string& uuidSrc, const std::string& uuidDst,
          size_t amountTraded);
    const std::string& getSrcOrder() const;
    const std::string& getDstOrder() const;
    size_t getAmountTraded() const;
    std::time_t getTradeTime() const;

   private:
    std::string uuidSrc_;
    std::string uuidDst_;
    size_t amountTraded_;  // сколько валюты перетекло в результате сделки
    std::time_t tradeTime;  // время совершения сделки
};