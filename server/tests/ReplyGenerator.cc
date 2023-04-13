#include "ReplyGenerator.h"

#include <gtest/gtest.h>

// #include <chrono>
// #include <thread>

class ReplyGeneratorFixture : public ::testing::Test {
   public:
    Core core;

   protected:
    void SetUp(void) override { core = Core(); }
};

// Проверка работы подключения
TEST_F(ReplyGeneratorFixture, HandleConnect_1) {
    ReplyGenerator replyGenerator(core);

    auto reply1 = replyGenerator.handleConnect("asd");
    auto reply2 = replyGenerator.handleConnect("zxc");
    auto reply3 = replyGenerator.handleConnect("asd");

    ASSERT_EQ(reply1, reply3);
    ASSERT_NE(reply1, reply2);
}

// Проверка работы транзакции
TEST_F(ReplyGeneratorFixture, HandleTransaction_1) {
    ReplyGenerator replyGenerator(core);

    auto id1 = replyGenerator.handleConnect("asd");

    auto reply1 = replyGenerator.handleTransaction(id1, 5, 50.0, Requests::Buy);
    auto reply2 = replyGenerator.handleTransaction("101", 5, 50.0, Requests::Buy);

    ASSERT_EQ(reply1, "Order successfully created");
    ASSERT_EQ(reply2, "Try to register first");
}

// Проверка работы запроса баланса клиента
TEST_F(ReplyGeneratorFixture, HandleBalance_1) {
    ReplyGenerator replyGenerator(core);

    auto id1 = replyGenerator.handleConnect("asd");
    auto id2 = replyGenerator.handleConnect("qwe");
    auto id3 = replyGenerator.handleConnect("zxc");

    auto reply1 = replyGenerator.handleTransaction(id1, 5, 50.0, Requests::Buy);
    auto reply2 = replyGenerator.handleTransaction(id2, 1, 49.0, Requests::Sell);
    auto reply3 = replyGenerator.handleTransaction(id3, 5, 50.0, Requests::Buy);

    auto json1 = nlohmann::json::parse(replyGenerator.handleBalance(id1));
    auto json2 = nlohmann::json::parse(replyGenerator.handleBalance(id2));
    auto json3 = nlohmann::json::parse(replyGenerator.handleBalance(id3));

    ASSERT_EQ(json1[Currency::RUR].get<double>(), -50);
    ASSERT_EQ(json1[Currency::USD].get<double>(), 1);

    ASSERT_EQ(json2[Currency::RUR].get<double>(), 49);
    ASSERT_EQ(json2[Currency::USD].get<double>(), -1);

    ASSERT_EQ(json3[Currency::RUR].get<double>(), 0);
    ASSERT_EQ(json3[Currency::USD].get<double>(), 0);
}

// Проверка работы истории совершенных сделок
TEST_F(ReplyGeneratorFixture, HandleHistory_1) {
    ReplyGenerator replyGenerator(core);
    auto id1 = replyGenerator.handleConnect("ASD");
    auto id2 = replyGenerator.handleConnect("Qwe");
    auto id3 = replyGenerator.handleConnect("ZXC");

    replyGenerator.handleTransaction(id1, 5, 50, Requests::Buy);
    replyGenerator.handleTransaction(id1, 5, 51, Requests::Buy);
    replyGenerator.handleTransaction(id2, 5, 52, Requests::Sell);
    replyGenerator.handleTransaction(id2, 9, 50, Requests::Sell);
    replyGenerator.handleTransaction(id3, 100, 100, Requests::Sell);

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    auto json1 = nlohmann::json::parse(replyGenerator.handleHistory(id1));
    auto json2 = nlohmann::json::parse(replyGenerator.handleHistory(id2));
    auto json3 = nlohmann::json::parse(replyGenerator.handleHistory(id3));

    ASSERT_EQ(json1.size(), 2);
    ASSERT_EQ(json2.size(), 2);
    ASSERT_EQ(json3.size(), 0);

    ASSERT_EQ(json1[0][OrderFields::Amount].get<size_t>(), 5u);
    ASSERT_EQ(json1.at(0)[OrderFields::Cost].get<double>(), 51.0);
    ASSERT_EQ(json1.at(0)[OrderFields::OperationType], Operation::BUY);

    ASSERT_EQ(json1.at(1)[OrderFields::Amount].get<size_t>(), 4u);
    ASSERT_EQ(json1.at(1)[OrderFields::Cost].get<double>(), 50.0);
    ASSERT_EQ(json1.at(1)[OrderFields::OperationType], Operation::BUY);

    ASSERT_EQ(json2.at(0)[OrderFields::Amount].get<size_t>(), 5u);
    ASSERT_EQ(json2.at(0)[OrderFields::Cost].get<double>(), 50.0);
    ASSERT_EQ(json2.at(0)[OrderFields::OperationType], Operation::SELL);

    ASSERT_EQ(json2.at(1)[OrderFields::Amount].get<size_t>(), 4u);
    ASSERT_EQ(json2.at(1)[OrderFields::Cost].get<double>(), 50.0);
    ASSERT_EQ(json2.at(1)[OrderFields::OperationType], Operation::SELL);
}

// Проверка работы активных сделок клиента
TEST_F(ReplyGeneratorFixture, HandleActive_1) {
    ReplyGenerator replyGenerator(core);

    auto id1 = replyGenerator.handleConnect("asd");
    auto id2 = replyGenerator.handleConnect("qwe");
    auto id3 = replyGenerator.handleConnect("zxc");
    auto id4 = replyGenerator.handleConnect("zxcd");

    auto reply1 = replyGenerator.handleTransaction(id1, 5, 50.0, Requests::Buy);
    auto reply2 = replyGenerator.handleTransaction(id1, 5, 100.1, Requests::Sell);
    auto reply3 = replyGenerator.handleTransaction(id2, 1, 49.0, Requests::Sell);
    auto reply4 = replyGenerator.handleTransaction(id3, 5, 50.0, Requests::Buy);

    auto json1 = nlohmann::json::parse(replyGenerator.handleActive(id1));
    auto json2 = nlohmann::json::parse(replyGenerator.handleActive(id2));
    auto json3 = nlohmann::json::parse(replyGenerator.handleActive(id3));
    auto json4 = nlohmann::json::parse(replyGenerator.handleActive(id4));

    ASSERT_EQ(json1[0][OrderFields::Amount].get<size_t>(), 4);
    ASSERT_EQ(json1[0][OrderFields::Cost].get<double>(), 50);
    ASSERT_EQ(json1[0][OrderFields::OperationType].get<std::string>(), Operation::BUY);

    ASSERT_EQ(json1[1][OrderFields::Amount].get<size_t>(), 5);
    ASSERT_EQ(json1[1][OrderFields::Cost].get<double>(), 100.1);
    ASSERT_EQ(json1[1][OrderFields::OperationType].get<std::string>(), Operation::SELL);

    ASSERT_EQ(json2.size(), 0);

    ASSERT_EQ(json3[0][OrderFields::Amount].get<size_t>(), 5);
    ASSERT_EQ(json3[0][OrderFields::Cost].get<double>(), 50.0);
    ASSERT_EQ(json3[0][OrderFields::OperationType].get<std::string>(), Operation::BUY);

    ASSERT_EQ(json4.size(), 0);
}
