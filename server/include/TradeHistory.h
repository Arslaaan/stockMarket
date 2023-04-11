#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>

#include "Order.h"
#include "Trade.h"

class TradeHistory {
   public:
    std::string add(std::shared_ptr<Order> orderSrc,
                    std::shared_ptr<Order> orderDst, size_t amountTraded);
    const std::shared_ptr<Trade>& get(const std::string& tradeId) const;

   private:
    std::unordered_map<std::string, std::shared_ptr<Trade>> trades;
    boost::uuids::random_generator generator;
};