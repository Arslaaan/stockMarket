#include "TradeHistory.h"

// @param orderSrc от кого идут деньги
// @param orderDst кому идут деньги
// @param amountTraded количество обменянной валюты
std::string TradeHistory::add(const std::unique_ptr<Order>& orderSrc,
                              const std::unique_ptr<Order>& orderDst,
                              size_t amountTraded) {
    std::string uuid = boost::uuids::to_string(generator());
    std::cout << "tradeId: {" << uuid << "}. Client '"
              << orderSrc->getClientId() << "'"
              << " sold " << amountTraded << " USD "
              << "to client "
              << "'" << orderDst->getClientId() << "'" << std::endl;
    trades[uuid] = std::make_unique<Trade>(orderSrc->getId(), orderDst->getId(),
                                           amountTraded);
    return uuid;
}

const std::unique_ptr<Trade>& TradeHistory::get(const std::string& tradeId) const {
    return trades.at(tradeId);
}
