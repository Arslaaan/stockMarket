#pragma once

#include <string>

static short port = 5555;

namespace Requests {
static std::string Connect = "Con";
static std::string Buy = "Buy";
static std::string Sell = "Sel";
}  // namespace Requests

namespace Currency {
    static std::string USD = "USD";
    static std::string RUR = "RUR";
}