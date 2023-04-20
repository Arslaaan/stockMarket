#pragma once

#include <deque>
#include <iostream>

// хранит все заявки с одинаковым cost и одинаковым типом покупки/продажа
// Заявки расположены в порядке добавления
class EqualCostOrders {
   public:
    explicit EqualCostOrders(const std::string& uuid);
    //Добавить заявку
    void add(const std::string& uuid);
    //Возвращает очередь id заявок
    const std::deque<std::string>& get() const;
    //Взять первую заявку
    const std::string& getFirst() const;
    //Убрать первую заявку
    void popFirst();
    //Есть ли заявки
    bool empty();
    //Возвращает количество заявок
    size_t size();

   private:
    std::deque<std::string> uuids; // orderIds
};