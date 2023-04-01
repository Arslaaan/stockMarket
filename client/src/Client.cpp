#include "Client.h"

using boost::asio::ip::tcp;

Client::Client(boost::asio::io_service &io_service) : aSocket(io_service) {}


// "Создаём" пользователя, получаем его ID.
void Client::processRegistration() {
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    // Для регистрации Id не нужен, заполним его нулём
    SendMessage(Requests::Registration, name);
    id = ReadMessage();
}

tcp::socket &Client::getSocket() { return aSocket; }

// Отправка сообщения на сервер по шаблону.
void Client::SendMessage(const std::string &aRequestType,
                         const std::string &aMessage) {
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

std::string Client::ReadMessage() {
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}
