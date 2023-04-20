#include <gtest/gtest.h>

#include <boost/beast/core/detail/base64.hpp>

#include "Core.h"

class TestFixture : public ::testing::Test {
   public:
    Core core;

   protected:
    void SetUp(void) override {
        core = Core();
        // заводим 5 пользователей
        core.registerNewUser("asd", "");
        core.registerNewUser("aaa", "");
        core.registerNewUser("bbb", "");
        core.registerNewUser("ccc", "");
        core.registerNewUser("zxc", "");
    }
};

// одна заявка на покупку
TEST_F(TestFixture, CreateOrder_1) {
    auto id = core.createOrder("1", 5, 70.0, true);

    ASSERT_EQ(core.getBuyHeap().size(), 1);
    ASSERT_EQ(core.getBuyHeap().top(), 70.0);

    const auto &buyOrders = core.getBuyOrders().at(70.0);
    ASSERT_EQ(buyOrders.get().size(), 1);
    ASSERT_EQ(buyOrders.get().front(), id);

    const auto &sellOrders = core.getSellOrders();
    ASSERT_TRUE(sellOrders.empty());
    ASSERT_TRUE(core.getSellHeap().empty());
}

// одна заявка на продажу
TEST_F(TestFixture, CreateOrder_2) {
    auto id = core.createOrder("1", 5, 70.0, false);

    ASSERT_EQ(core.getSellHeap().size(), 1);
    ASSERT_EQ(core.getSellHeap().top(), 70.0);

    const auto &sellOrders = core.getSellOrders().at(70.0);
    ASSERT_EQ(sellOrders.get().size(), 1);
    ASSERT_EQ(sellOrders.get().front(), id);

    const auto &buyOrders = core.getBuyOrders();
    ASSERT_TRUE(buyOrders.empty());
    ASSERT_TRUE(core.getBuyHeap().empty());
}

// комплексный тест на множество заявок
TEST_F(TestFixture, CreateOrder_3) {
    core.createOrder("1", 5, 70.0, true);
    core.createOrder("1", 5, 70.0, true);
    core.createOrder("1", 5, 72.0, true);

    core.createOrder("2", 5, 70.0, false);
    core.createOrder("2", 5, 73.0, false);
    core.createOrder("2", 5, 73.0, false);

    ASSERT_EQ(core.getSellHeap().size(), 2);
    ASSERT_EQ(core.getBuyHeap().size(), 2);

    ASSERT_EQ(core.getSellHeap().top(), 70.0);
    ASSERT_EQ(core.getBuyHeap().top(), 72.0);

    ASSERT_EQ(core.getSellOrders().at(70.0).get().size(), 1);
    ASSERT_EQ(core.getSellOrders().at(73.0).get().size(), 2);

    ASSERT_EQ(core.getBuyOrders().at(70.0).get().size(), 2);
    ASSERT_EQ(core.getBuyOrders().at(72.0).get().size(), 1);
}

// тест на регистрацию пользователя
TEST_F(TestFixture, RegisterUsers_1) {
    std::string password = "qwerty";
    core.registerNewUser(
        "Arslan",
        Auth::encode64(
            password));  // encode делаем на стороне клиента, а тут так

    boost::hash<std::string> hasher;
    std::string hashedPass = std::to_string(hasher(password));

    auto id = core.getUserId("Arslan").value();
    ASSERT_TRUE(
        core.getClientsInfo().at(id).checkAuth(Auth::encode64(hashedPass)));
}

// тест на двух пользователей
TEST_F(TestFixture, RegisterUsers_2) {
    core.registerNewUser("Arslan", "");
    core.registerNewUser("Arslan1", "");

    auto id1 = core.getUserId("Arslan").value();
    auto id2 = core.getUserId("Arslan1").value();
    ASSERT_EQ(core.getClientsInfo().at(id1).getUserName(), "Arslan");
    ASSERT_EQ(core.getClientsInfo().at(id2).getUserName(), "Arslan1");
    ASSERT_NE(id1, id2);
}

// одна заявка, матчить не с кем
TEST_F(TestFixture, Match1) {
    core.createOrder("1", 5, 50.5, true);

    core.match();

    ASSERT_EQ(core.getBuyOrders().size(), 1);
    ASSERT_EQ(core.getSellOrders().size(), 0);

    ASSERT_EQ(core.getBuyHeap().size(), 1);
    ASSERT_EQ(core.getSellHeap().size(), 0);
}

// одна заявка, матчить не с кем
TEST_F(TestFixture, Match_2) {
    core.createOrder("1", 5, 50.5, false);

    core.match();

    ASSERT_EQ(core.getBuyOrders().size(), 0);
    ASSERT_EQ(core.getSellOrders().size(), 1);

    ASSERT_EQ(core.getBuyHeap().size(), 0);
    ASSERT_EQ(core.getSellHeap().size(), 1);
}

// 4 заявки, но сделка 1
TEST_F(TestFixture, Match_3) {
    core.createOrder("1", 5, 100, false);
    core.createOrder("1", 5, 60, false);
    core.createOrder("2", 5, 60, true);
    core.createOrder("2", 5, 50.5, true);

    core.match();

    ASSERT_EQ(core.getBuyOrders().size(), 1);
    ASSERT_EQ(core.getSellOrders().size(), 1);

    ASSERT_EQ(core.getBuyHeap().size(), 1);
    ASSERT_EQ(core.getSellHeap().size(), 1);

    ASSERT_EQ(core.getSellHeap().top(), 100);
    ASSERT_EQ(core.getBuyHeap().top(), 50.5);
}

// пример из описания задачи
TEST_F(TestFixture, Match_4) {
    // todo добавить моки, чтобы проверять последовательность исполнения заявок
    // через нотифай
    core.createOrder("1", 10, 62, true);
    core.createOrder("2", 20, 63, true);
    auto id = core.createOrder(
        "3", 50, 61, false);  // оставшаяся заявка после частичного исполнения

    core.match();

    ASSERT_EQ(core.getBuyOrders().size(), 0);
    ASSERT_EQ(core.getSellOrders().size(), 1);
    ASSERT_EQ(core.getOrderKeeper().get(id)->getAmount(), 20);
    ASSERT_EQ(core.getOrderKeeper().get(id)->getCost(), 61);

    ASSERT_EQ(core.getBuyHeap().size(), 0);
    ASSERT_EQ(core.getSellHeap().size(), 1);

    ASSERT_EQ(core.getSellHeap().top(), 61);
}

// Покупка одной заявки через 5 частично выполненных заявок
TEST_F(TestFixture, Match_5) {
    core.createOrder("2", 50, 63, true);
    core.createOrder("1", 10, 60, false);
    core.createOrder("1", 10, 60, false);
    core.createOrder("1", 10, 60, false);
    core.createOrder("1", 10, 60, false);
    core.createOrder("1", 10, 60, false);

    core.match();

    ASSERT_EQ(core.getBuyOrders().size(), 0);
    ASSERT_EQ(core.getSellOrders().size(), 0);

    ASSERT_EQ(core.getBuyHeap().size(), 0);
    ASSERT_EQ(core.getSellHeap().size(), 0);
}

// 2 клиента постепенно пришли к сделке
TEST_F(TestFixture, Match_6) {
    core.createOrder("1", 1, 63, true);
    core.createOrder("1", 1, 64, true);
    core.createOrder("1", 1, 65, true);
    core.createOrder("1", 1, 66, true);
    core.createOrder("1", 1, 67, true);

    core.createOrder("2", 1, 71, false);
    core.createOrder("2", 1, 70, false);
    core.createOrder("2", 1, 69, false);
    core.createOrder("2", 1, 68, false);
    core.createOrder("2", 1, 67, false);

    core.match();

    ASSERT_EQ(core.getBuyOrders().size(), 4);
    ASSERT_EQ(core.getSellOrders().size(), 4);

    ASSERT_EQ(core.getBuyHeap().size(), 4);
    ASSERT_EQ(core.getSellHeap().size(), 4);
}

// Проверка что более ранняя заявка обслуживается раньше при прочих равных
TEST_F(TestFixture, Match_7) {
    core.createOrder("1", 1, 70, true);
    auto id1 = core.createOrder("2", 5, 65, false);
    auto id2 = core.createOrder("2", 5, 65, false);

    core.match();

    ASSERT_EQ(core.getOrderKeeper().get(id1)->getAmount(), 4);
    ASSERT_EQ(core.getOrderKeeper().get(id2)->getAmount(), 5);
}

// комплексный тест на проверку заполнения clientInfo + истории сделок
TEST_F(TestFixture, ClientInfo_1) {
    auto id1 = core.createOrder("1", 1, 70, true);
    auto id2 = core.createOrder("2", 2, 65, false);
    auto id3 = core.createOrder("3", 3, 65, false);
    auto id4 = core.createOrder("4", 3, 65, true);

    core.match();

    ASSERT_EQ(core.getBuyHeap().size(), 0);
    ASSERT_EQ(core.getSellHeap().size(), 1);

    auto clientInfo1 = core.getClientsInfo().at("1");
    ASSERT_EQ(clientInfo1.getBalance(Currency::RUR), -70);
    ASSERT_EQ(clientInfo1.getBalance(Currency::USD), 1);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo1.getHistory().at(0))
                  ->getSrcOrder(),
              id2);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo1.getHistory().at(0))
                  ->getDstOrder(),
              id1);
    ASSERT_EQ(clientInfo1.getActiveOrders().size(), 0);

    auto clientInfo2 = core.getClientsInfo().at("2");
    ASSERT_EQ(clientInfo2.getBalance(Currency::RUR), 135);
    ASSERT_EQ(clientInfo2.getBalance(Currency::USD), -2);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo2.getHistory().at(0))
                  ->getSrcOrder(),
              id2);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo2.getHistory().at(0))
                  ->getDstOrder(),
              id1);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo2.getHistory().at(1))
                  ->getDstOrder(),
              id4);
    ASSERT_EQ(clientInfo2.getActiveOrders().size(), 0);

    auto clientInfo3 = core.getClientsInfo().at("3");
    ASSERT_EQ(clientInfo3.getBalance(Currency::RUR), 130);
    ASSERT_EQ(clientInfo3.getBalance(Currency::USD), -2);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo3.getHistory().at(0))
                  ->getSrcOrder(),
              id3);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo3.getHistory().at(0))
                  ->getDstOrder(),
              id4);
    ASSERT_EQ(clientInfo3.getActiveOrders().size(), 1);

    auto clientInfo4 = core.getClientsInfo().at("4");
    ASSERT_EQ(clientInfo4.getBalance(Currency::RUR), -195);
    ASSERT_EQ(clientInfo4.getBalance(Currency::USD), 3);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo4.getHistory().at(0))
                  ->getSrcOrder(),
              id2);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo4.getHistory().at(0))
                  ->getDstOrder(),
              id4);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo4.getHistory().at(1))
                  ->getSrcOrder(),
              id3);
    ASSERT_EQ(core.getTradeHistory()
                  .get(clientInfo4.getHistory().at(1))
                  ->getDstOrder(),
              id4);
    ASSERT_EQ(clientInfo4.getActiveOrders().size(), 0);
}
