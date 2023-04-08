#include "NotificationService.h"

void NotificationService::notify(std::shared_ptr<Order> order,
                                 size_t amountChanged) {
    std::string operation = " sold ";
    if (order->isBuyOrder()) {
        operation = " bought ";
    }
    std::cout << "Client with id " << order->getClientId() << operation
              << amountChanged << " USD by " << order->getCost() << " RUB";
}
