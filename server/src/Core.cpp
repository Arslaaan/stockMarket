#include "Core.h"

std::string Core::registerNewUser(const std::string &aUserName) {
    size_t newUserId = mUsers.size();
    mUsers[newUserId] = aUserName;
    mUserNames[aUserName] = newUserId;

    return std::to_string(newUserId);
}

// для уменьшения дублирования кода
template <class T, class M>
std::optional<M> findUser(const T &user, std::map<T, M> &map) {
    const auto userIt = map.find(user);
    if (userIt == map.cend()) {
        return {};
    }
    return userIt->second;
}

// Запрос имени клиента по ID
std::optional<std::string> Core::getUserName(const std::string &aUserId) {
    int id = std::stoi(aUserId);
    return findUser<size_t, std::string>(id, mUsers);
}

// Запрос ID клиента по имени
std::optional<std::string> Core::getUserId(const std::string &aUserName) {
    auto userId = findUser<std::string, size_t>(aUserName, mUserNames);
    if (userId.has_value()) {
        return std::to_string(userId.value());
    }
    return {};
}

void Core::match() {
    while (!buyHeap.empty() && !sellHeap.empty() &&
           (buyHeap.top() - sellHeap.top() >= 0)) {
        auto &buyOrdersWithMaxCost = buyOrders[buyHeap.top()].get();
        auto &sellOrdersWithMinCost = sellOrders[sellHeap.top()].get();
        auto buyOrder = orderKeeper.get(*buyOrdersWithMaxCost.begin());
        auto sellOrder = orderKeeper.get(*sellOrdersWithMinCost.begin());

        if (buyOrder->getAmount() > sellOrder->getAmount()) {
            updateOrderFull(sellOrder);
            updateOrderPartially(
                buyOrder, buyOrder->getAmount() - sellOrder->getAmount());
        } else if (buyOrder->getAmount() < sellOrder->getAmount()) {
            updateOrderFull(buyOrder);
            updateOrderPartially(
                sellOrder, sellOrder->getAmount() - buyOrder->getAmount());
        } else {
            updateOrderFull(sellOrder);
            updateOrderFull(buyOrder);
        }
    }
}

void Core::createOrder(const std::string &clientId, size_t amount, double cost,
                       bool isBuy) {
    const std::string &orderId =
        orderKeeper.save(clientId, amount, cost, isBuy);
    std::string orderType = "sell ";
    auto &correspondingMap = sellOrders;
    if (isBuy) {
        correspondingMap = buyOrders;
        orderType = "buy ";
    }
    if (correspondingMap.count(cost) > 0) {
        correspondingMap[cost].add(orderId);
    } else {
        correspondingMap[cost] =
            EqualCostOrders(isBuy, orderId);  // todo проверить!
        isBuy ? buyHeap.push(cost) : sellHeap.push(cost);
    }
    std::cout << "User with id " << clientId << " creates order to "
              << orderType << amount << " USD by " << cost << " RUB"
              << std::endl;
}

// Частично реализует предложение в заявке
void Core::updateOrderPartially(std::shared_ptr<Order> order,
                                size_t amountExchanged) {
    order->setAmount(order->getAmount() - amountExchanged);
    notificationService.notify(order, amountExchanged);
}

// Полностью реализует предложение в заявке
void Core::updateOrderFull(std::shared_ptr<Order> order) {
    auto &mapOfOrders = sellOrders;
    if (order->isBuyOrder()) {
        mapOfOrders = buyOrders;
    }

    auto &uuidDeque = mapOfOrders[order->getCost()].get();
    uuidDeque.pop_front();
    if (uuidDeque.empty()) {
        mapOfOrders.erase(order->getCost());
        order->isBuyOrder() ? buyHeap.pop() : sellHeap.pop();
    }
    notificationService.notify(order, order->getCost());
    orderKeeper.remove(order->getId());
}

OrderKeeper &Core::getOrderKeeper() { return orderKeeper; }

Core &GetCore() {
    static Core core;
    return core;
}
