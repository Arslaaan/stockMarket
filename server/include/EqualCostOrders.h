#pragma once

#include <deque>
#include <iostream>

// хранит все заявки с одинаковым cost и типом покупки/продажа
class EqualCostOrders {
   public:
    explicit EqualCostOrders(const std::string& uuid);
    //Добавить заявку
    void add(const std::string& uuid);
    //Возвращает очередь id заявок
    const std::deque<std::string>& get() const;
    const std::string& getFirst() const;
    void popFirst();
    bool empty();
    //Возвращает количество заявок
    size_t size();

   private:
    std::deque<std::string> uuids;
};