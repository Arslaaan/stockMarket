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
        auto buyOrder = orderKeeper.get(buyOrders.at(buyHeap.top()).getFirst());
        auto sellOrder =
            orderKeeper.get(sellOrders.at(sellHeap.top()).getFirst());
        trade(sellOrder, buyOrder);
    }
}

std::string Core::createOrder(const std::string &clientId, size_t amount,
                              double cost, bool isBuy) {
    const std::string &orderId =
        orderKeeper.save(clientId, amount, cost, isBuy);
    std::string orderType = isBuy ? "buy" : "sell";
    auto &correspondingMap = isBuy ? buyOrders : sellOrders;

    if (correspondingMap.count(cost) > 0) {
        correspondingMap.at(cost).add(orderId);
    } else {
        correspondingMap.emplace(cost, orderId);
        isBuy ? buyHeap.push(cost) : sellHeap.push(cost);
    }
    clientsInfo[clientId].addActiveOrder(orderId);
    std::cout << "User with id " << clientId << " creates order to "
              << orderType << " " << amount << " USD by " << cost << " RUB"
              << std::endl;
    return orderId;
}

// @param orderSrc от кого идет валюта
// @param orderDst кому идет валюта
// @param amountTraded количество обменянной валюты
void Core::trade(std::shared_ptr<Order> orderSrc,
                 std::shared_ptr<Order> orderDst) {
    size_t amount;
    if (orderDst->getAmount() > orderSrc->getAmount()) {
        amount = orderSrc->getAmount();
        updateOrderFull(orderSrc);
        updateOrderPartially(orderDst, amount);
    } else if (orderDst->getAmount() < orderSrc->getAmount()) {
        amount = orderDst->getAmount();
        updateOrderFull(orderDst);
        updateOrderPartially(orderSrc, amount);
    } else {
        amount = orderSrc->getAmount();
        updateOrderFull(orderSrc);
        updateOrderFull(orderDst);
    }
    auto tradeId = tradeHistory.add(orderSrc, orderDst, amount);
    //todo где то тут ошибка
    updateClientInfo(orderSrc, tradeId, true, amount);
    updateClientInfo(orderDst, tradeId, false, amount);
}

/// @brief Обновить информацию о клиенте
/// @param clientId
/// @param tradeId
/// @param sellCurrency true - продаем валюту, иначе покупаем
/// @param amount количество валюты для обмена
void Core::updateClientInfo(std::shared_ptr<Order> &order,
                            const std::string &tradeId, bool sellCurrency,
                            double amount) {
    ClientInfo &clientInfo = clientsInfo.at(order->getClientId());
    if (sellCurrency) {
        clientInfo.addToBalance(Currency::RUR, amount * order->getCost());
        clientInfo.takeFromBalance(Currency::USD, amount);
    } else {
        clientInfo.addToBalance(Currency::USD, amount);
        clientInfo.takeFromBalance(Currency::RUR, amount * order->getCost());
    }
    clientInfo.historyUpdate(tradeId);
    if (!order->isActiveOrder()) {
        // значит заявку деактивировали, то есть полностью реализовали
        clientInfo.removeOrder(order->getId());
    }
}

// Частично реализует предложение в заявке
void Core::updateOrderPartially(std::shared_ptr<Order> order,
                                size_t amountTraded) {
    order->setAmount(order->getAmount() - amountTraded);
}

// Полностью реализует предложение в заявке
void Core::updateOrderFull(std::shared_ptr<Order> order) {
    auto &mapOfOrders = order->isBuyOrder() ? buyOrders : sellOrders;

    EqualCostOrders &equalCostOrders = mapOfOrders.at(order->getCost());
    equalCostOrders.popFirst();
    if (equalCostOrders.empty()) {
        mapOfOrders.erase(order->getCost());
        order->isBuyOrder() ? buyHeap.pop() : sellHeap.pop();
    }
    order->deactivate();
}

OrderKeeper &Core::getOrderKeeper() { return orderKeeper; }

const std::priority_queue<double, std::vector<double>, std::less<>>
    &Core::getBuyHeap() const {
    return buyHeap;
}

const std::priority_queue<double, std::vector<double>, std::greater<>>
    &Core::getSellHeap() const {
    return sellHeap;
}

const std::unordered_map<double, EqualCostOrders> &Core::getBuyOrders() const {
    return buyOrders;
}

const std::unordered_map<double, EqualCostOrders> &Core::getSellOrders() const {
    return sellOrders;
}

const std::unordered_map<std::string, ClientInfo> &Core::getClientsInfo()
    const {
    return clientsInfo;
}

const TradeHistory &Core::getTradeHistory() const { return tradeHistory; }

void Core::clear() {
    orderKeeper.clear();
    // notificationService.clear();
    sellOrders.clear();
    buyOrders.clear();
    while (!sellHeap.empty()) {
        sellHeap.pop();
    }
    while (!buyHeap.empty()) {
        buyHeap.pop();
    }
    mUserNames.clear();
    mUsers.clear();
}

Core &GetCore() {
    static Core core;
    return core;
}
