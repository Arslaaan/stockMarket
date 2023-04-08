#pragma once

#include <deque>
#include <iostream>

// хранит все заявки с одинаковым cost и типом покупки/продажа
class EqualCostOrders {
   public:
    EqualCostOrders();
    EqualCostOrders(bool isBuy_, const std::string& uuid);
    //Добавить заявку
    void add(const std::string& uuid);
    //Возвращает очередь id заявок
    std::deque<std::string>& get();
    //Возвращает количество заявок
    size_t size();

   private:
    bool isBuy;
    std::deque<std::string> uuids;
};