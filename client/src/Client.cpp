#include "Client.h"

using boost::asio::ip::tcp;

Client::Client(boost::asio::io_service &io_service) : aSocket(io_service) {}

// Подключаемся либо создаём, если такого пользователя нет, получаем его ID.
void Client::connect(const std::string &name) {
    SendMessage(Requests::Connect, name);
    id = ReadMessage();
    std::cout << "You have assigned id: " << getId() << std::endl;
}

void Client::openOrder(const std::string &type, const std::string &amount,
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

void Client::balance() {
    SendMessage(Requests::Balance, "");
    std::string answer = ReadMessage();
    auto json = nlohmann::json::parse(answer);
    std::cout << "Current balance: [" << Currency::USD << ": "
              << json[Currency::USD] << "] "
              << "[" << Currency::RUR << ": " << json[Currency::RUR] << "]"
              << std::endl;
}

void Client::history() {
    SendMessage(Requests::History, "");
    std::string answer = ReadMessage();
    auto json = nlohmann::json::parse(answer);

    std::cout << "History of all completed trades sorted from old to new:"
              << std::endl;
    for (auto &elem : json) {
        size_t amount = elem[OrderFields::Amount].get<size_t>();
        double cost = elem[OrderFields::Cost].get<double>();
        std::string operationType =
            elem[OrderFields::OperationType] == Operation::BUY ? "buy" : "sell";
        std::time_t openTime = elem[OrderFields::OpenTime].get<time_t>();
        std::string openTimeString = std::ctime(&openTime);
        openTimeString.pop_back();
        std::time_t closeTime = elem[OrderFields::CloseTime].get<time_t>();
        std::string closeTimeString = std::ctime(&closeTime);
        closeTimeString.pop_back();

        std::cout << "\t"
                  << "Time open/close: [" << openTimeString << " - "
                  << closeTimeString << "], order: [" << operationType << " "
                  << amount << " USD by cost: " << cost << "]" << std::endl;
    }
}

void Client::active() {
    SendMessage(Requests::Active, "");
    std::string answer = ReadMessage();
    auto json = nlohmann::json::parse(answer);

    std::cout << "Active orders sorted from old to new:" << std::endl;
    for (auto &elem : json) {
        size_t amount = elem[OrderFields::Amount].get<size_t>();
        double cost = elem[OrderFields::Cost].get<double>();
        std::string operationType =
            elem[OrderFields::OperationType] == Operation::BUY ? "buy" : "sell";
        std::time_t openTime = elem[OrderFields::OpenTime].get<time_t>();
        std::string openTimeString = std::ctime(&openTime);
        openTimeString.pop_back();

        std::cout << "\t"
                  << "Time open: [" << openTimeString << "], order: ["
                  << operationType << " " << amount << " USD by cost: " << cost
                  << "]" << std::endl;
    }
}

tcp::socket &Client::getSocket() { return aSocket; }

const std::string &Client::getId() const { return id; }

// Отправка сообщения на сервер по шаблону.
void Client::SendMessage(const std::string &aRequestType,
                         const std::string &aMessage) {
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = aRequestType;
    if (!aMessage.empty()) {
        req["Message"] = aMessage;
    }

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
