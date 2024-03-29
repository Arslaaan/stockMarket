#include "Core.h"

void Core::registerNewUser(const std::string &aUserName,
                           const std::string &passEncoded) {
    size_t newUserId = mUserNames.size();
    mUserNames[aUserName] = newUserId;

    const auto &stringUserId = std::to_string(newUserId);
    const auto &decodedPass = Auth::decode64(passEncoded);
    const auto &auth = Auth::encode64(hash(decodedPass));

    clientsInfo.emplace(std::piecewise_construct,
                        std::forward_as_tuple(stringUserId),
                        std::forward_as_tuple(stringUserId, aUserName, auth));
}

template <class T, class M>
std::optional<M> findUser(const T &user, std::map<T, M> &map) {
    const auto userIt = map.find(user);
    if (userIt == map.cend()) {
        return {};
    }
    return userIt->second;
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
        if (!buyOrders.count(buyHeap.top())) {
            // если не находим - значит заявку отменили
            buyHeap.pop();
            continue;
        }
        if (!sellOrders.count(sellHeap.top())) {
            // если не находим - значит заявку отменили
            sellHeap.pop();
            continue;
        }
        const auto &sellOrder =
            orderKeeper.get(sellOrders.at(sellHeap.top()).getFirst());
        const auto &buyOrder =
            orderKeeper.get(buyOrders.at(buyHeap.top()).getFirst());
        trade(sellOrder, buyOrder);
    }
}

void Core::removeOldOrder(const std::unique_ptr<Order> &order) {
    closeOrder(order);
    clientsInfo.at(order->getClientId()).removeOrder(order->getId());
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
    clientsInfo.at(clientId).addActiveOrder(orderId);
    std::cout << "orderId: {" << orderId << "}. "
              << "User with id " << clientId << " creates order to "
              << orderType << " " << amount << " USD by " << cost << " RUB"
              << std::endl;
    return orderId;
}

// @param orderSrc от кого идет валюта
// @param orderDst кому идет валюта
// @param amountTraded количество обменянной валюты
void Core::trade(const std::unique_ptr<Order> &orderSrc,
                 const std::unique_ptr<Order> &orderDst) {
    size_t amount;
    if (orderDst->getAmount() > orderSrc->getAmount()) {
        amount = orderSrc->getAmount();
        closeOrder(orderSrc);
        closeOrderPartially(orderDst, amount);
    } else if (orderDst->getAmount() < orderSrc->getAmount()) {
        amount = orderDst->getAmount();
        closeOrder(orderDst);
        closeOrderPartially(orderSrc, amount);
    } else {
        amount = orderSrc->getAmount();
        closeOrder(orderSrc);
        closeOrder(orderDst);
    }
    const auto &tradeId = tradeHistory.add(orderSrc, orderDst, amount);
    bestCost =
        std::min(orderSrc, orderDst,
                 [](const std::unique_ptr<Order> &src,
                    const std::unique_ptr<Order> &dst) {
                     return src->getTimestamp() < dst->getTimestamp();
                 })
            ->getCost();
    updateClientInfo(orderSrc, tradeId, true, amount, bestCost);
    updateClientInfo(orderDst, tradeId, false, amount, bestCost);
}

/// @brief Обновить информацию о клиенте
/// @param clientId
/// @param tradeId
/// @param sellCurrency true - продаем валюту, иначе покупаем
/// @param amount количество валюты для обмена
/// @param cost по какой цене меняют валюту
void Core::updateClientInfo(const std::unique_ptr<Order> &order,
                            const std::string &tradeId, bool sellCurrency,
                            size_t amount, double cost) {
    ClientInfo &clientInfo = clientsInfo.at(order->getClientId());
    if (sellCurrency) {
        clientInfo.addToBalance(Currency::RUR, amount * cost);
        clientInfo.takeFromBalance(Currency::USD, amount);
    } else {
        clientInfo.addToBalance(Currency::USD, amount);
        clientInfo.takeFromBalance(Currency::RUR, amount * cost);
    }
    clientInfo.historyUpdate(tradeId);
    if (!order->isActiveOrder()) {
        // значит заявку деактивировали, то есть полностью реализовали
        clientInfo.removeOrder(order->getId());
    }
}

// Частично реализует предложение в заявке
void Core::closeOrderPartially(const std::unique_ptr<Order> &order,
                               size_t amountTraded) {
    order->setAmount(order->getAmount() - amountTraded);
}

// Полностью реализует предложение в заявке
void Core::closeOrder(const std::unique_ptr<Order> &order) {
    auto &mapOfOrders = order->isBuyOrder() ? buyOrders : sellOrders;

    EqualCostOrders &equalCostOrders = mapOfOrders.at(order->getCost());
    equalCostOrders.popFirst();
    if (equalCostOrders.empty()) {
        mapOfOrders.erase(order->getCost());
        // актуализируем соответствующую heap после закрытия заявки, если можем.
        // Это не всегда возможно, тк клиент может отменить заявку cost которой
        // не на вершине heap, в таком случае она актуализируется позже при
        // Core::match
        if (order->isBuyOrder()) {
            if (buyHeap.top() == order->getCost()) {
                buyHeap.pop();
            }
        } else {
            if (sellHeap.top() == order->getCost()) {
                sellHeap.pop();
            }
        }
    }
    order->deactivate();
}

std::string Core::hash(const std::string &text) {
    return std::to_string(hasher(text));
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

const std::string Core::getQuotes() const {
    std::stringstream ss;
    ss << "Last trade cost: ";
    if (bestCost > 0.0) {
        ss << bestCost << " " << Currency::RUR;
    } else {
        ss << "None";
    }

    return ss.str();
}

Core &GetCore() {
    static Core core;
    return core;
}
