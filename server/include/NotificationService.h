#pragma once

#include <memory>
#include "Order.h"
#include <iostream>

class NotificationService {
    public:
    // Сообщить клиентам информацию о изменениях по заявке.
    // @param order - заявка
    // @param amountChanged - количество купленной/проданной валюты
    void notify(std::shared_ptr<Order> order, size_t amountChanged);
    private:
};