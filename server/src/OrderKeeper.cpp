#include "OrderKeeper.h"

std::string OrderKeeper::save(const std::string& clientId, size_t amount,
                              double cost, bool isBuy) {
    std::string uuid = boost::uuids::to_string(generator());
    // std::cout << "Saved order with id " << uuid << std::endl;
    orders[uuid] = std::make_unique<Order>(uuid, clientId, amount, cost, isBuy);
    return uuid;
};

const std::unique_ptr<Order>& OrderKeeper::get(const std::string& uuid) {
    return orders.at(uuid);
}

void OrderKeeper::remove(const std::string& uuid) { orders.erase(uuid); }

const std::unique_ptr<Order>& OrderKeeper::get(
    const std::unique_ptr<Trade>& trade, const std::string userId) {
    const auto& orderSrc = orders[trade->getSrcOrder()];
    const auto& orderDst = orders[trade->getDstOrder()];
    if (orderSrc->getClientId() == userId) {
        return orderSrc;
    } else {
        return orderDst;
    }
}
