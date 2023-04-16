#include "ReplyGenerator.h"

ReplyGenerator::ReplyGenerator(Core& core) : core_(core) {}

std::string ReplyGenerator::handleLogin(const std::string& userName,
                                        const std::string& pass) {
    auto userId = core_.getUserId(userName);
    if (userId.has_value()) {
        const std::string& hash_ = core_.hash(Auth::decode64(pass));
        const std::string& auth = Auth::encode64(hash_);
        if (core_.getClientsInfo()
                .at(userId.value())
                .checkAuth(auth)) {  // cверка пароля
            nlohmann::json answer;
            answer["Id"] = userId.value();
            answer["Auth"] = auth;
            return answer.dump();
        } else {
            return "Log in failed";
        }
    }
    return "Try to register first";
}

std::string ReplyGenerator::handleRegister(const std::string& userName,
                                           const std::string& auth) {
    auto createdUserId = core_.getUserId(userName);
    if (!createdUserId.has_value()) {
        core_.registerNewUser(userName, auth);
        return "Registration successful";
    }
    return "User already exists";
}

std::string ReplyGenerator::handleTransaction(const std::string& userId,
                                              size_t amount, double cost,
                                              const std::string& type) {
    core_.createOrder(userId, amount, cost, type == Requests::Buy);
    core_.match();
    return "Order successfully created";
}

std::string ReplyGenerator::handleBalance(const std::string& userId) {
    nlohmann::json answer;
    const ClientInfo& clientInfo = core_.getClientsInfo().at(userId);
    answer[Currency::RUR] = clientInfo.getBalance(Currency::RUR);
    answer[Currency::USD] = clientInfo.getBalance(Currency::USD);
    return answer.dump();
}

std::string ReplyGenerator::handleHistory(const std::string& userId) {
    const ClientInfo& clientInfo = core_.getClientsInfo().at(userId);
    std::vector<nlohmann::json> items;
    for (const auto& tradeId : clientInfo.getHistory()) {
        const auto& trade = core_.getTradeHistory().get(tradeId);
        nlohmann::json item;
        const auto& order = core_.getOrderKeeper().get(trade, userId);
        item[OrderFields::OperationType] =
            order->isBuyOrder() ? Operation::BUY : Operation::SELL;
        item[OrderFields::OpenTime] = order->getTimestamp();
        item[OrderFields::CloseTime] = trade->getTradeTime();
        item[OrderFields::Amount] = trade->getAmountTraded();
        item[OrderFields::Cost] = order->getCost();
        items.push_back(item);
    }
    nlohmann::json answer = items;
    return answer.dump();
}

std::string ReplyGenerator::handleActive(const std::string& userId) {
    const ClientInfo& clientInfo = core_.getClientsInfo().at(userId);
    std::vector<nlohmann::json> items;
    for (const auto& orderId : clientInfo.getActiveOrders()) {
        nlohmann::json item;
        const auto& order = core_.getOrderKeeper().get(orderId);
        item[OrderFields::OperationType] =
            order->isBuyOrder() ? Operation::BUY : Operation::SELL;
        item[OrderFields::OpenTime] = order->getTimestamp();
        item[OrderFields::Amount] = order->getAmount();
        item[OrderFields::Cost] = order->getCost();
        items.push_back(item);
    }
    nlohmann::json answer = items;
    return answer.dump();
}

std::optional<std::string> ReplyGenerator::checkUser(const std::string& userId,
                                                     const std::string& auth) {
    if (!core_.getClientsInfo().count(userId)) {
        return "Try to register first";
    }
    if (!core_.getClientsInfo().at(userId).checkAuth(auth)) {
        return "Unauthorized access";
    }
    return {};
}
