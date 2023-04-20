#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "Order.h"
#include "Trade.h"

// Хранит информацию по всем заявкам
class OrderKeeper {
   public:
    // сохранить заявку
    std::string save(const std::string& clientId, size_t amount, double cost,
                     bool isBuy);
    // получить заявку
    const std::unique_ptr<Order>& get(const std::string& uuid);
    // удалить заявку
    void remove(const std::string& uuid);
    // получить заявку из сделки по userId
    std::pair<const Order*, const Order*> get(
        const std::unique_ptr<Trade>& trade, const std::string userId);

   private:
    std::unordered_map<std::string, std::unique_ptr<Order>> orders;
    boost::uuids::random_generator generator;
};