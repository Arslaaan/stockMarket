#include "OrderKeeper.h"

std::string OrderKeeper::save(const std::string& clientId, size_t amount, double cost, bool isBuy) {
    std::string uuid = boost::uuids::to_string(generator());
    std::cout << "Saved order with id " << uuid << std::endl;
    orders[uuid] = std::make_shared<Order>(uuid, clientId, amount, cost, isBuy);
    return uuid;
};

std::shared_ptr<Order> OrderKeeper::get(const std::string& uuid) {
    return orders[uuid];
}

void OrderKeeper::remove(const std::string& uuid) {
    orders.erase(uuid);
}

void OrderKeeper::clear() {
    orders.clear();
}
