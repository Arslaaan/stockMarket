#include "Client.h"

using boost::asio::ip::tcp;

Client::Client(boost::asio::io_service &io_service) : aSocket(io_service) {}

void Client::login(const std::string &name, const std::string &pass) {
    auth = Auth::encode64(pass);
    SendMessage(Requests::Login, name);
    const auto &answer = ReadMessage();
    try {
        auto json = nlohmann::json::parse(answer);
        id = json["Id"];
        auth = json["Auth"];
        std::cout << "Login successful!" << std::endl;
    } catch (const std::exception &e) {
        std::cout << "Server: " << answer << std::endl;
    }
}

void Client::registration(const std::string &name, const std::string &pass) {
    SendMessage(Requests::Register, name, Auth::encode64(pass));
    const auto &answer = ReadMessage();
    std::cout << "Server: " << answer << std::endl;
}

void Client::openOrder(const std::string &type, const std::string &amount,
                       const std::string &cost) {
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = type;
    req["Amount"] = amount;
    req["Cost"] = cost;
    req["Auth"] = auth;

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
    try {
        auto json = nlohmann::json::parse(answer);
        std::cout << "Current balance: [" << Currency::USD << ": "
                  << json[Currency::USD] << "] "
                  << "[" << Currency::RUR << ": " << json[Currency::RUR] << "]"
                  << std::endl;
    } catch (const std::exception &e) {
        std::cout << "Server: " << answer << std::endl;
    }
}

void Client::quote() {
    SendMessage(Requests::Quote, "");
    std::string answer = ReadMessage();
    std::cout << "Server: " << answer << std::endl;
}

void Client::history() {
    SendMessage(Requests::History, "");
    const std::string &answer = ReadMessage();
    try {
        auto json = nlohmann::json::parse(answer);
        std::cout << "History of all completed trades sorted from old to new:"
                  << std::endl;
        for (auto &elem : json) {
            size_t amount = elem[OrderFields::Amount].get<size_t>();
            double cost = elem[OrderFields::Cost].get<double>();
            std::string operationType =
                elem[OrderFields::OperationType] == Operation::BUY ? "buy"
                                                                   : "sell";
            std::time_t openTime = elem[OrderFields::OpenTime].get<time_t>();
            std::string openTimeString = std::ctime(&openTime);
            openTimeString.pop_back();
            std::time_t closeTime = elem[OrderFields::CloseTime].get<time_t>();
            std::string closeTimeString = std::ctime(&closeTime);
            closeTimeString.pop_back();

            std::cout << "\t"
                      << "Time open/close: [" << openTimeString << " - "
                      << closeTimeString << "], order: [" << operationType
                      << " " << amount << " USD by cost: " << cost << "]"
                      << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Server: " << answer << std::endl;
    }
}

void Client::active() {
    SendMessage(Requests::Active, "");
    const std::string &answer = ReadMessage();

    try {
        auto json = nlohmann::json::parse(answer);
        std::cout << "Active orders sorted from old to new:" << std::endl;
        for (auto &elem : json) {
            size_t amount = elem[OrderFields::Amount].get<size_t>();
            double cost = elem[OrderFields::Cost].get<double>();
            std::string operationType =
                elem[OrderFields::OperationType] == Operation::BUY ? "buy"
                                                                   : "sell";
            std::time_t openTime = elem[OrderFields::OpenTime].get<time_t>();
            std::string openTimeString = std::ctime(&openTime);
            openTimeString.pop_back();

            std::cout << "orderId: {" << elem[OrderFields::OrderId] << "} - "
                      << "time open: [" << openTimeString << "], operation: ["
                      << operationType << " " << amount
                      << " USD by cost: " << cost << "]" << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Server: " << answer << std::endl;
    }
}

void Client::cancel(const std::string &orderId) {
    SendMessage(Requests::Cancel, orderId);
    std::string answer = ReadMessage();
    std::cout << "Server: " << answer << std::endl;
}

tcp::socket &Client::getSocket() { return aSocket; }

const std::string &Client::getId() const { return id; }

// Отправка сообщения на сервер по шаблону.
void Client::SendMessage(const std::string &aRequestType,
                         const std::string &aMessage,
                         const std::string &auth_) {
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = aRequestType;
    if (!aMessage.empty()) {
        req["Message"] = aMessage;
    }
    req["Auth"] = auth_;

    std::string request = req.dump();
    boost::system::error_code error;
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()),
                       error);
    if (error) {
        std::cout << "Sending the message failed" << std::endl;
    }
}

// Отправка сообщения на сервер по шаблону.
void Client::SendMessage(const std::string &aRequestType,
                         const std::string &aMessage) {
    SendMessage(aRequestType, aMessage, auth);
}

std::string Client::ReadMessage() {
    boost::asio::streambuf b;
    boost::system::error_code error;
    boost::asio::read_until(aSocket, b, "\0", error);
    if (error) {
        return "no respond";
    }
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}
