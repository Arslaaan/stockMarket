#include "EqualCostOrders.h"

EqualCostOrders::EqualCostOrders(const std::string& uuid)
    {
    uuids.push_back(uuid);
}

void EqualCostOrders::add(const std::string& uuid) { uuids.push_back(uuid); }

const std::deque<std::string>& EqualCostOrders::get() const { return uuids; }

const std::string& EqualCostOrders::getFirst() const { return uuids.front(); }

void EqualCostOrders::popFirst() {
    uuids.pop_front();
}

bool EqualCostOrders::empty() { return uuids.empty(); }

size_t EqualCostOrders::size() { return uuids.size(); }
