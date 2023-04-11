#include "NotificationService.h"

void NotificationService::notify(std::shared_ptr<Order> order,
                                 size_t amountChanged) {
    // todo доработать уведомление так, чтобы сообщалась с кем была совершена сделка
    std::string operation = " sold ";
    if (order->isBuyOrder()) {
        operation = " bought ";
    }
    std::cout << "Client with id " << order->getClientId() << operation
              << amountChanged << " USD by " << order->getCost() << " RUB"
              << std::endl;
}
