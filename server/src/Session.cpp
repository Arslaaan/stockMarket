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
        return;
    }

    data_[bytes_transferred] = '\0';
    ReplyGenerator replyGenerator(GetCore());
    std::string reply = "Error! Unknown request type";

    try {
        // Парсим json, который пришёл нам в сообщении.
        auto j = nlohmann::json::parse(data_);
        std::string userId = j["UserId"];
        auto reqType = j["ReqType"];

        if (reqType == Requests::Connect) {
            reply = replyGenerator.handleConnect(j["Message"]);
        } else if (reqType == Requests::Buy || reqType == Requests::Sell) {
            size_t amount = std::stoul(j["Amount"].get<std::string>());
            double cost = std::stod(j["Cost"].get<std::string>());
            reply =
                replyGenerator.handleTransaction(userId, amount, cost, reqType);
        } else if (reqType == Requests::Balance) {
            reply = replyGenerator.handleBalance(userId);
        } else if (reqType == Requests::History) {
            reply = replyGenerator.handleHistory(userId);
        } else if (reqType == Requests::Active) {
            reply = replyGenerator.handleActive(userId);
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
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
