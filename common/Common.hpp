#pragma once

#include <string>

static short port = 5555;

namespace Requests {
static std::string Connect = "Con";
static std::string Buy = "Buy";
static std::string Sell = "Sel";
static std::string Balance = "Bal";
static std::string History = "Hst";
static std::string Active = "Act";
}  // namespace Requests

namespace Currency {
static std::string USD = "USD";
static std::string RUR = "RUR";
}  // namespace Currency

namespace OrderFields {
static std::string OpenTime = "ot";
static std::string CloseTime = "ct";
static std::string OperationType = "op";  // -> "b" or "s"
static std::string Amount = "am";
static std::string Cost = "co";
}  // namespace OrderFields

namespace Operation {
static std::string BUY = "b";
static std::string SELL = "s";
}  // namespace Operation