#include "EqualCostOrders.h"

EqualCostOrders::EqualCostOrders() = default;

EqualCostOrders::EqualCostOrders(bool isBuy_, const std::string& uuid)
    : isBuy(isBuy_) {
    uuids.push_back(uuid);
}

void EqualCostOrders::add(const std::string& uuid) { uuids.push_back(uuid); }

std::deque<std::string>& EqualCostOrders::get() { return uuids; }

size_t EqualCostOrders::size() { return uuids.size(); }
