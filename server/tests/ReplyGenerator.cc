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

    replyGenerator.handleRegister("asd", Auth::encode64("s"));
    replyGenerator.handleRegister("zxc", Auth::encode64("s"));
    auto reply =
        replyGenerator.handleLogin("asd", Auth::encode64("s"));  // логин

    ASSERT_NE("Log in failed", reply);
    ASSERT_NE("Try to register first", reply);
}

// Проверка работы подключения с неправильным паролем
TEST_F(ReplyGeneratorFixture, HandleConnect_2) {
    ReplyGenerator replyGenerator(core);

    auto reply1 = replyGenerator.handleRegister("asd", Auth::encode64("sa"));
    auto reply2 = replyGenerator.handleLogin("asd", Auth::encode64("s"));

    ASSERT_EQ("Registration successful", reply1);
    ASSERT_EQ(reply2, "Log in failed");
}

// Обертка для теста, чтобы уменьшить дублирование кода
nlohmann::json handleLogin(ReplyGenerator& replyGenerator,
                           const std::string& name, const ::std::string& pass) {
    return nlohmann::json::parse(
        replyGenerator.handleLogin(name, Auth::encode64(pass)));
}

// Проверка работы транзакции
TEST_F(ReplyGeneratorFixture, HandleTransaction_1) {
    ReplyGenerator replyGenerator(core);

    replyGenerator.handleRegister("asd", Auth::encode64(""));
    auto id = handleLogin(replyGenerator, "asd", "")["Id"];

    auto reply1 = replyGenerator.handleTransaction(id, 5, 50.0, Requests::Buy);

    ASSERT_EQ(reply1, "Order successfully created");
}

// Проверка работы запроса баланса клиента
TEST_F(ReplyGeneratorFixture, HandleBalance_1) {
    ReplyGenerator replyGenerator(core);

    replyGenerator.handleRegister("asd", Auth::encode64(""));
    replyGenerator.handleRegister("qwe", Auth::encode64(""));
    replyGenerator.handleRegister("zxc", Auth::encode64(""));

    auto id1 = handleLogin(replyGenerator, "asd", "")["Id"];
    auto id2 = handleLogin(replyGenerator, "qwe", "")["Id"];
    auto id3 = handleLogin(replyGenerator, "zxc", "")["Id"];

    auto reply1 = replyGenerator.handleTransaction(id1, 5, 50.0, Requests::Buy);
    auto reply2 =
        replyGenerator.handleTransaction(id2, 1, 49.0, Requests::Sell);
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
    replyGenerator.handleRegister("ASD", Auth::encode64("q"));
    replyGenerator.handleRegister("Qwe", Auth::encode64("q"));
    replyGenerator.handleRegister("zxc", Auth::encode64("q"));

    auto id1 = handleLogin(replyGenerator, "ASD", "q")["Id"];
    auto id2 = handleLogin(replyGenerator, "Qwe", "q")["Id"];
    auto id3 = handleLogin(replyGenerator, "zxc", "q")["Id"];

    replyGenerator.handleTransaction(id1, 5, 50, Requests::Buy);
    replyGenerator.handleTransaction(id1, 5, 51, Requests::Buy);
    replyGenerator.handleTransaction(id2, 5, 52, Requests::Sell);
    replyGenerator.handleTransaction(id2, 9, 50, Requests::Sell);
    replyGenerator.handleTransaction(id3, 100, 100, Requests::Sell);

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

    replyGenerator.handleRegister("asd", Auth::encode64("q"));
    replyGenerator.handleRegister("sd", Auth::encode64("q"));
    replyGenerator.handleRegister("asda", Auth::encode64("q"));
    replyGenerator.handleRegister("asdd", Auth::encode64("q"));

    auto id1 = handleLogin(replyGenerator, "asd", "q")["Id"];
    auto id2 = handleLogin(replyGenerator, "sd", "q")["Id"];
    auto id3 = handleLogin(replyGenerator, "asda", "q")["Id"];
    auto id4 = handleLogin(replyGenerator, "asdd", "q")["Id"];

    auto reply1 = replyGenerator.handleTransaction(id1, 5, 50.0, Requests::Buy);
    auto reply2 =
        replyGenerator.handleTransaction(id1, 5, 100.1, Requests::Sell);
    auto reply3 =
        replyGenerator.handleTransaction(id2, 1, 49.0, Requests::Sell);
    auto reply4 = replyGenerator.handleTransaction(id3, 5, 50.0, Requests::Buy);

    auto json1 = nlohmann::json::parse(replyGenerator.handleActive(id1));
    auto json2 = nlohmann::json::parse(replyGenerator.handleActive(id2));
    auto json3 = nlohmann::json::parse(replyGenerator.handleActive(id3));
    auto json4 = nlohmann::json::parse(replyGenerator.handleActive(id4));

    ASSERT_EQ(json1[0][OrderFields::Amount].get<size_t>(), 4);
    ASSERT_EQ(json1[0][OrderFields::Cost].get<double>(), 50);
    ASSERT_EQ(json1[0][OrderFields::OperationType].get<std::string>(),
              Operation::BUY);

    ASSERT_EQ(json1[1][OrderFields::Amount].get<size_t>(), 5);
    ASSERT_EQ(json1[1][OrderFields::Cost].get<double>(), 100.1);
    ASSERT_EQ(json1[1][OrderFields::OperationType].get<std::string>(),
              Operation::SELL);

    ASSERT_EQ(json2.size(), 0);

    ASSERT_EQ(json3[0][OrderFields::Amount].get<size_t>(), 5);
    ASSERT_EQ(json3[0][OrderFields::Cost].get<double>(), 50.0);
    ASSERT_EQ(json3[0][OrderFields::OperationType].get<std::string>(),
              Operation::BUY);

    ASSERT_EQ(json4.size(), 0);
}

// Проверка зарегистрирован ли пользователь / верность пароля
TEST_F(ReplyGeneratorFixture, CheckUser_1) {
    ReplyGenerator replyGenerator(core);

    replyGenerator.handleRegister("asd", Auth::encode64("q"));
    auto answer = handleLogin(replyGenerator, "asd", "q");
    auto id = answer["Id"];
    auto token = answer["Auth"];

    ASSERT_FALSE(replyGenerator.checkUser(id, token).has_value());
    ASSERT_EQ(replyGenerator.checkUser("100", token).value(),
              "Try to register first");  // незарегистрированный пользователь
    ASSERT_EQ(replyGenerator
                  .checkUser(id, "123123121451515")  // не тот токен
                  .value(),
              "Unauthorized access");  // не тот хэш
}

// Проверка котировок
TEST_F(ReplyGeneratorFixture, CheckQuote_1) {
    ReplyGenerator replyGenerator(core);

    replyGenerator.handleRegister("asd", Auth::encode64("q"));
    auto id1 = handleLogin(replyGenerator, "asd", "q")["Id"];

    replyGenerator.handleRegister("qwe", Auth::encode64("q"));
    auto id2 = handleLogin(replyGenerator, "qwe", "q")["Id"];

    replyGenerator.handleTransaction(id1, 5, 51, Requests::Buy);
    replyGenerator.handleTransaction(id2, 4, 50, Requests::Sell);

    ASSERT_EQ(
        "Last completed sell cost: 50 RUR, last completed buy cost: 51 RUR",
        replyGenerator.handleQuote());
}

// Проверка котировок если сделок не было
TEST_F(ReplyGeneratorFixture, CheckQuote_2) {
    ReplyGenerator replyGenerator(core);

    replyGenerator.handleRegister("asd", Auth::encode64("q"));
    auto id1 = handleLogin(replyGenerator, "asd", "q")["Id"];

    replyGenerator.handleRegister("qwe", Auth::encode64("q"));
    auto id2 = handleLogin(replyGenerator, "qwe", "q")["Id"];

    ASSERT_EQ("Last completed sell cost: None, last completed buy cost: None",
              replyGenerator.handleQuote());
}

// Проверка отмены заявки если она на вершине кучи
TEST_F(ReplyGeneratorFixture, CheckCancel_1) {
    ReplyGenerator replyGenerator(core);

    replyGenerator.handleRegister("asd", Auth::encode64("q"));
    auto id1 = handleLogin(replyGenerator, "asd", "q")["Id"];

    replyGenerator.handleTransaction(id1, 100, 50.0, Requests::Buy);
    replyGenerator.handleTransaction(id1, 50, 80.0, Requests::Sell);
    replyGenerator.handleTransaction(id1, 60, 60.0, Requests::Buy);
    auto orderId1 = core.getClientsInfo().at(id1).getActiveOrders().at(0);
    auto orderId2 = core.getClientsInfo().at(id1).getActiveOrders().at(1);
    auto orderId3 = core.getClientsInfo().at(id1).getActiveOrders().at(2);
    replyGenerator.handleCancel(orderId2);

    ASSERT_EQ(core.getClientsInfo().at(id1).getActiveOrders().size(), 2);
    ASSERT_EQ(core.getClientsInfo().at(id1).getActiveOrders().at(0), orderId1);
    ASSERT_FALSE(core.getOrderKeeper().get(orderId2)->isActiveOrder());
    ASSERT_EQ(core.getClientsInfo().at(id1).getActiveOrders().at(1), orderId3);
}

// Проверка отмены заявки если она НЕ на вершине кучи
TEST_F(ReplyGeneratorFixture, CheckCancel_2) {
    ReplyGenerator replyGenerator(core);

    replyGenerator.handleRegister("asd", Auth::encode64("q"));
    auto id1 = handleLogin(replyGenerator, "asd", "q")["Id"];
    replyGenerator.handleRegister("asdd", Auth::encode64("q"));
    auto id2 = handleLogin(replyGenerator, "asdd", "q")["Id"];

    auto orderId1 = core.createOrder(id1, 10, 60.0, true);
    auto orderId2 = core.createOrder(id1, 10, 70.0, true);
    auto orderId3 = core.createOrder(id1, 10, 80.0, true);
    
    auto otherOrderId1 = core.createOrder(id2, 10, 60.0, false);
    auto otherOrderId2 = core.createOrder(id2, 10, 70.0, false);
    auto otherOrderId3 = core.createOrder(id2, 10, 80.0, false);

    ASSERT_EQ("Order succesfully canceled", replyGenerator.handleCancel(orderId2));

    core.match();

    ASSERT_EQ(core.getClientsInfo().at(id1).getActiveOrders().size(), 1);
    ASSERT_EQ(core.getClientsInfo().at(id1).getActiveOrders().at(0), orderId1);
    ASSERT_TRUE(core.getOrderKeeper().get(orderId1)->isActiveOrder());
    ASSERT_FALSE(core.getOrderKeeper().get(orderId2)->isActiveOrder());
    ASSERT_FALSE(core.getOrderKeeper().get(orderId3)->isActiveOrder());

    ASSERT_EQ(core.getClientsInfo().at(id2).getActiveOrders().size(), 2);
    ASSERT_EQ(core.getClientsInfo().at(id2).getActiveOrders().at(0), otherOrderId2);
    ASSERT_EQ(core.getClientsInfo().at(id2).getActiveOrders().at(1), otherOrderId3);
    ASSERT_TRUE(core.getOrderKeeper().get(otherOrderId2)->isActiveOrder());
    ASSERT_TRUE(core.getOrderKeeper().get(otherOrderId3)->isActiveOrder());
}

// Проверка если такой заявки нет
TEST_F(ReplyGeneratorFixture, CheckCancel_3) {
    ReplyGenerator replyGenerator(core);

    replyGenerator.handleRegister("asd", Auth::encode64("q"));
    auto id1 = handleLogin(replyGenerator, "asd", "q")["Id"];

    auto orderId1 = core.createOrder(id1, 10, 60.0, true);
    
    ASSERT_EQ("No order found", replyGenerator.handleCancel(orderId1 + "1"));
}
