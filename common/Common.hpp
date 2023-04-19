#pragma once

#include <string>

static short port = 5555;

namespace Requests {
static std::string Login = "Log"; // войти в аккаунт
static std::string Register = "Rgr"; // зарегистрироваться
static std::string Buy = "Buy"; // купить
static std::string Sell = "Sel"; // продать
static std::string Balance = "Bal"; // получить баланс
static std::string History = "Hst"; // история совершенных сделок
static std::string Active = "Act"; // активные сделки
static std::string Quote = "Qte"; // получить текующую котировку
static std::string Cancel = "Cnl"; // отменить одну из текущих заявок
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
static std::string OrderId = "od";
}  // namespace OrderFields

namespace Operation {
static std::string BUY = "b";
static std::string SELL = "s";
}  // namespace Operation

#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

namespace Auth {
static std::string decode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It =
        transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return boost::algorithm::trim_right_copy_if(
        std::string(It(std::begin(val)), It(std::end(val))),
        [](char c) { return c == '\0'; });
}
static std::string encode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It =
        base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
}
}  // namespace Auth