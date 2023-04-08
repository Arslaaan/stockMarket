#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>

#include "Order.h"
#include <boost/uuid/uuid.hpp>            
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 

//Хранит информацию по всем заявкам
class OrderKeeper {
   public:
    //сохранить заявку
    std::string save(const std::string& clientId, size_t amount, double cost, bool isBuy);
    //получить заявку
    std::shared_ptr<Order> get(const std::string& uuid);
    //удалить заявку
    void remove(const std::string& uuid);

   private:
    std::unordered_map<std::string, std::shared_ptr<Order>> orders;
    boost::uuids::random_generator generator;
};