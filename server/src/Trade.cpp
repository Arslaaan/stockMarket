#include "Trade.h"

Trade::Trade(const std::string& uuidSrc, const std::string& uuidDst,
             size_t amountTraded)
    : uuidSrc_(uuidSrc),
      uuidDst_(uuidDst),
      amountTraded_(amountTraded),
      tradeTime(std::time(nullptr)) {}

const std::string& Trade::getSrcOrder() const { return uuidSrc_; }

const std::string& Trade::getDstOrder() const { return uuidDst_; }

size_t Trade::getAmountTraded() const { return amountTraded_; }

std::time_t Trade::getTradeTime() const { return tradeTime; }
