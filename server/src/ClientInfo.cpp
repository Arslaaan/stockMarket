#include "ClientInfo.h"

ClientInfo::ClientInfo(const std::string& userId_, const std::string& userName_,
                       const std::string& auth_)
    : userId(userId_), userName(userName_), auth(auth_) {
    balance_[Currency::USD] = 0.0;
    balance_[Currency::RUR] = 0.0;
}

void ClientInfo::addToBalance(const std::string& type, double income) {
    balance_[type] += income;
}
double ClientInfo::getBalance(const std::string& type) const {
    return balance_.at(type);
}

const std::string& ClientInfo::getUserName() const { return userName; }

bool ClientInfo::checkAuth(const std::string& auth_) const {
    return auth == auth_;
}

void ClientInfo::takeFromBalance(const std::string& type, double expenditure) {
    balance_[type] -= expenditure;
}

void ClientInfo::historyUpdate(const std::string& tradeId) {
    history_.push_back(tradeId);
}

const std::vector<std::string>& ClientInfo::getHistory() const {
    return history_;
}

const std::vector<std::string>& ClientInfo::getActiveOrders() const {
    return activeOrders_;
}

void ClientInfo::addActiveOrder(const std::string& orderId) {
    activeOrders_.push_back(orderId);
}

void ClientInfo::removeOrder(const std::string& orderId) {
    // todo написать юнит-тест
    auto it = std::remove_if(
        activeOrders_.begin(), activeOrders_.end(),
        [orderId](const std::string& aOrder) { return aOrder == orderId; });
    activeOrders_.erase(it, activeOrders_.end());
}
