#include "Order.h"

Order::Order(const std::string& id_, const std::string& clientId_,
             size_t amount_, double cost_, bool isBuy_)
    : id(id_),
      clientId(clientId_),
      amount(amount_),
      cost(cost_),
      isBuy(isBuy_),
      timestamp(std::time(nullptr)),
      isActive(true) {}
const std::string& Order::getId() const { return id; }
const std::string& Order::getClientId() const { return clientId; }
const std::time_t& Order::getTimestamp() const { return timestamp; }
size_t Order::getAmount() const { return amount; }
void Order::setAmount(size_t newAmount) { amount = newAmount; }
double Order::getCost() const { return cost; }
bool Order::isBuyOrder() const { return isBuy; }
bool Order::isActiveOrder() const { return isActive; }
void Order::deactivate() { isActive = false; };