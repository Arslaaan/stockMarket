#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>

#include "Order.h"
#include "Trade.h"

class TradeHistory {
   public:
    std::string add(const std::unique_ptr<Order>& orderSrc,
                    const std::unique_ptr<Order>& orderDst, size_t amountTraded);
    const std::unique_ptr<Trade>& get(const std::string& tradeId) const;

   private:
    std::unordered_map<std::string, std::unique_ptr<Trade>> trades;
    boost::uuids::random_generator generator;
};