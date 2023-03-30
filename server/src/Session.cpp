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
    if (!error) {
        data_[bytes_transferred] = '\0';

        // Парсим json, который пришёл нам в сообщении.
        auto j = nlohmann::json::parse(data_);
        auto reqType = j["ReqType"];

        std::string reply = "Error! Unknown request type";
        if (reqType == Requests::Registration) {
            // Это реквест на регистрацию пользователя.
            // Добавляем нового пользователя и возвращаем его ID.
            reply = GetCore().RegisterNewUser(j["Message"]);
        } else if (reqType == Requests::Hello) {
            // Это реквест на приветствие.
            // Находим имя пользователя по ID и приветствуем его по имени.
            reply = "Hello, " + GetCore().GetUserName(j["UserId"]) + "!\n";
        }

        boost::asio::async_write(socket_,
                                 boost::asio::buffer(reply, reply.size()),
                                 boost::bind(&Session::handle_write, this,
                                             boost::asio::placeholders::error));
    } else {
        delete this;
    }
}

void Session::handle_write(const boost::system::error_code &error) {
    if (!error) {
        socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            boost::bind(&Session::handle_read, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else {
        delete this;
    }
}
