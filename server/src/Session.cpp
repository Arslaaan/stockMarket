#include "Session.h"

Session::Session(boost::asio::io_service &io_service) : socket_(io_service) {}

tcp::socket &Session::socket() { return socket_; }

void Session::start() {
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        boost::bind(&Session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Session::handle_read(const boost::system::error_code &error,
                          size_t bytes_transferred) {
    if (error) {
        delete this;
    }

    data_[bytes_transferred] = '\0';

    // Парсим json, который пришёл нам в сообщении.
    auto j = nlohmann::json::parse(data_);
    auto reqType = j["ReqType"];

    std::string reply = "Error! Unknown request type";
    std::string userId = j["UserId"];
    if (reqType == Requests::Connect) {
        // Это реквест на авторизацию пользователя (возможно нового).
        // Возвращаем его ID.
        auto userIdInMsg = GetCore().getUserId(j["Message"]);
        if (!userIdInMsg.has_value()) {
            reply = GetCore().registerNewUser(j["Message"]);
        } else {
            reply = userIdInMsg.value();
        }
    } else if (reqType == Requests::Buy || reqType == Requests::Sell) {
        size_t amount = std::stoul(j["Amount"].get<std::string>());
        double cost = std::stod(j["Cost"].get<std::string>());
        GetCore().createOrder(userId, amount, cost, reqType == Requests::Buy);
        GetCore().match();
        reply = "Success";
    }

    boost::asio::async_write(socket_, boost::asio::buffer(reply, reply.size()),
                             boost::bind(&Session::handle_write, this,
                                         boost::asio::placeholders::error));
}

void Session::handle_write(const boost::system::error_code &error) {
    if (error) {
        delete this;
    }
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        boost::bind(&Session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}
