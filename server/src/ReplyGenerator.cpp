#include "ReplyGenerator.h"

ReplyGenerator::ReplyGenerator(Core& core) : core_(core) {}

std::string ReplyGenerator::handleConnect(const std::string& userName) {
    auto createdUserId = core_.getUserId(userName);
    return createdUserId.has_value() ? createdUserId.value()
                                     : core_.registerNewUser(userName);
}

std::string ReplyGenerator::handleTransaction(const std::string& userId,
                                              size_t amount, double cost,
                                              const std::string& type) {
    if (!core_.getUserName(userId).has_value()) {
        return "Try to register first";
    }
    core_.createOrder(userId, amount, cost, type == Requests::Buy);
    core_.match();
    return "Order successfully created";
}

std::string ReplyGenerator::handleBalance(const std::string& userId) {
    nlohmann::json answer;
    std::optional<ClientInfo> clientInfo = core_.getClientsInfo().at(userId);
    if (clientInfo.has_value()) {
        answer[Currency::RUR] = clientInfo.value().getBalance(Currency::RUR);
        answer[Currency::USD] = clientInfo.value().getBalance(Currency::USD);
        return answer.dump();
    }
    return "Try to register first";
}

std::string ReplyGenerator::handleHistory(const std::string& userId) {
    std::optional<ClientInfo> clientInfo = core_.getClientsInfo().at(userId);
    std::vector<nlohmann::json> items;
    if (clientInfo.has_value()) {
        for (const auto& tradeId : clientInfo->getHistory()) {
            auto trade = core_.getTradeHistory().get(tradeId);
            nlohmann::json item;
            std::shared_ptr<Order> order = core_.getOrderKeeper().get(trade->getSrcOrder());
            item[OrderFields::OperationType] = Operation::SELL;
            if (order->getClientId() != userId) {
                order = core_.getOrderKeeper().get(trade->getDstOrder());
                item[OrderFields::OperationType] = Operation::BUY;
            }
            item[OrderFields::OpenTime] = order->getTimestamp();
            item[OrderFields::CloseTime] = trade->getTradeTime();
            item[OrderFields::Amount] = trade->getAmountTraded();
            item[OrderFields::Cost] = order->getCost();
            items.push_back(item);
        }
        nlohmann::json answer = items;
        return answer.dump();
    }
    return "Try to register first";
}

std::string ReplyGenerator::handleActive(const std::string& userId) {
    std::optional<ClientInfo> clientInfo = core_.getClientsInfo().at(userId);
    std::vector<nlohmann::json> items;
    if (clientInfo.has_value()) {
        for (const auto& orderId : clientInfo->getActiveOrders()) {
            nlohmann::json item;
            std::shared_ptr<Order> order = core_.getOrderKeeper().get(orderId);
            item[OrderFields::OperationType] = order->isBuyOrder() ? Operation::BUY : Operation::SELL;
            item[OrderFields::OpenTime] = order->getTimestamp();
            item[OrderFields::Amount] = order->getAmount();
            item[OrderFields::Cost] = order->getCost();
            items.push_back(item);
        }
        nlohmann::json answer = items;
        return answer.dump();
    }
    return "Try to register first";
}