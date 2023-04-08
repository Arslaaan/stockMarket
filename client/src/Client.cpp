#include "Client.h"

using boost::asio::ip::tcp;

Client::Client(boost::asio::io_service &io_service) : aSocket(io_service) {}

// Подключаемся либо создаём, если такого нет, пользователя, получаем его ID.
void Client::connect(const std::string &name) {
    // Для регистрации Id не нужен, заполним его нулём
    SendMessage(Requests::Connect, name);
    id = ReadMessage();
    std::cout << "You have assigned id: " << getId() << std::endl;
}

void Client::transaction(const std::string &type, const std::string &amount,
                         const std::string &cost) {
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = type;
    req["Amount"] = amount;
    req["Cost"] = cost;

    std::string request = req.dump();
    boost::system::error_code error;
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()),
                       error);
    if (error) {
        std::cout << "Transaction failed" << std::endl;
    } else {
        std::cout << this->ReadMessage() << std::endl;
    }
}

tcp::socket &Client::getSocket() { return aSocket; }

const std::string &Client::getId() const {
    return id;
}

// Отправка сообщения на сервер по шаблону.
void Client::SendMessage(const std::string &aRequestType,
                         const std::string &aMessage) {
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

    std::string request = req.dump();
    boost::system::error_code error;
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()),
                       error);
    if (error) {
        std::cout << "Sending the message failed" << std::endl;
    }
}

std::string Client::ReadMessage() {
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}
