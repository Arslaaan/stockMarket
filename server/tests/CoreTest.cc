#include <gtest/gtest.h>

#include "Core.h"

class TestFixture : public ::testing::Test {
   protected:
    void SetUp(void) override  // вызывается после конструктора перед тестом
    {
        GetCore().clear();
    }
};

// одна заявка на покупку
TEST_F(TestFixture, CreateOrder_1) {
    auto id = GetCore().createOrder("1", 5, 70.0, true);

    ASSERT_EQ(GetCore().getBuyHeap().size(), 1);
    ASSERT_EQ(GetCore().getBuyHeap().top(), 70.0);

    const auto& buyOrders = GetCore().getBuyOrders().at(70.0);
    ASSERT_EQ(buyOrders.get().size(), 1);
    ASSERT_EQ(buyOrders.get().front(), id);

    const auto& sellOrders = GetCore().getSellOrders();
    ASSERT_TRUE(sellOrders.empty());
    ASSERT_TRUE(GetCore().getSellHeap().empty());
}

// одна заявка на продажу
TEST_F(TestFixture, CreateOrder_2) {
    auto id = GetCore().createOrder("1", 5, 70.0, false);

    ASSERT_EQ(GetCore().getSellHeap().size(), 1);
    ASSERT_EQ(GetCore().getSellHeap().top(), 70.0);

    const auto& sellOrders = GetCore().getSellOrders().at(70.0);
    ASSERT_EQ(sellOrders.get().size(), 1);
    ASSERT_EQ(sellOrders.get().front(), id);

    const auto& buyOrders = GetCore().getBuyOrders();
    // ASSERT_TRUE(buyOrders.empty());
    ASSERT_TRUE(GetCore().getBuyHeap().empty());
}

// комплексный тест на множество заявок
TEST_F(TestFixture, CreateOrder_3) {
    GetCore().createOrder("1", 5, 70.0, true);
    GetCore().createOrder("1", 5, 70.0, true);
    GetCore().createOrder("1", 5, 72.0, true);

    GetCore().createOrder("2", 5, 70.0, false);
    GetCore().createOrder("2", 5, 73.0, false);
    GetCore().createOrder("2", 5, 73.0, false);

    ASSERT_EQ(GetCore().getSellHeap().size(), 2);
    ASSERT_EQ(GetCore().getBuyHeap().size(), 2);

    ASSERT_EQ(GetCore().getSellHeap().top(), 70.0);
    ASSERT_EQ(GetCore().getBuyHeap().top(), 72.0);

    ASSERT_EQ(GetCore().getSellOrders().at(70.0).get().size(), 1);
    ASSERT_EQ(GetCore().getSellOrders().at(73.0).get().size(), 2);

    ASSERT_EQ(GetCore().getBuyOrders().at(70.0).get().size(), 2);
    ASSERT_EQ(GetCore().getBuyOrders().at(72.0).get().size(), 1);
}

// тест на двух пользователей
TEST_F(TestFixture, RegisterUsers) {
    auto id1 = GetCore().registerNewUser("Arslan");
    auto id2 = GetCore().registerNewUser("Arslan1");

    ASSERT_EQ(GetCore().getUserId("Arslan").value(), id1);
    ASSERT_EQ(GetCore().getUserId("Arslan1").value(), id2);
    ASSERT_EQ(GetCore().getUserName(id1).value(), "Arslan");
    ASSERT_EQ(GetCore().getUserName(id2).value(), "Arslan1");
    ASSERT_NE(id1, id2);
}

// одна заявка, матчить не с кем
TEST_F(TestFixture, Match1) {
    GetCore().createOrder("1", 5, 50.5, true);

    GetCore().match();

    ASSERT_EQ(GetCore().getBuyOrders().size(), 1);
    ASSERT_EQ(GetCore().getSellOrders().size(), 0);

    ASSERT_EQ(GetCore().getBuyHeap().size(), 1);
    ASSERT_EQ(GetCore().getSellHeap().size(), 0);
}

// одна заявка, матчить не с кем
TEST_F(TestFixture, Match_2) {
    GetCore().createOrder("1", 5, 50.5, false);

    GetCore().match();

    ASSERT_EQ(GetCore().getBuyOrders().size(), 0);
    ASSERT_EQ(GetCore().getSellOrders().size(), 1);

    ASSERT_EQ(GetCore().getBuyHeap().size(), 0);
    ASSERT_EQ(GetCore().getSellHeap().size(), 1);
}

// 4 заявки, но сделка 1
TEST_F(TestFixture, Match_3) {
    GetCore().createOrder("1", 5, 100, false);
    GetCore().createOrder("1", 5, 60, false);
    GetCore().createOrder("2", 5, 60, true);
    GetCore().createOrder("2", 5, 50.5, true);

    GetCore().match();

    ASSERT_EQ(GetCore().getBuyOrders().size(), 1);
    ASSERT_EQ(GetCore().getSellOrders().size(), 1);

    ASSERT_EQ(GetCore().getBuyHeap().size(), 1);
    ASSERT_EQ(GetCore().getSellHeap().size(), 1);

    ASSERT_EQ(GetCore().getSellHeap().top(), 100);
    ASSERT_EQ(GetCore().getBuyHeap().top(), 50.5);
}

// пример из описания задачи
TEST_F(TestFixture, Match_4) {
    // todo добавить моки, чтобы проверять последовательность исполнения заявок
    // через нотифай
    GetCore().createOrder("1", 10, 62, true);
    GetCore().createOrder("2", 20, 63, true);
    auto id = GetCore().createOrder(
        "3", 50, 61, false);  // оставшаяся заявка после частичного исполнения

    GetCore().match();

    ASSERT_EQ(GetCore().getBuyOrders().size(), 0);
    ASSERT_EQ(GetCore().getSellOrders().size(), 1);
    ASSERT_EQ(GetCore().getOrderKeeper().get(id)->getAmount(), 20);
    ASSERT_EQ(GetCore().getOrderKeeper().get(id)->getCost(), 61);

    ASSERT_EQ(GetCore().getBuyHeap().size(), 0);
    ASSERT_EQ(GetCore().getSellHeap().size(), 1);

    ASSERT_EQ(GetCore().getSellHeap().top(), 61);
}

// Покупка одной заявки через 5 частично выполненных заявок
TEST_F(TestFixture, Match_5) {
    GetCore().createOrder("2", 50, 63, true);
    GetCore().createOrder("1", 10, 60, false);
    GetCore().createOrder("1", 10, 60, false);
    GetCore().createOrder("1", 10, 60, false);
    GetCore().createOrder("1", 10, 60, false);
    GetCore().createOrder("1", 10, 60, false);

    GetCore().match();

    ASSERT_EQ(GetCore().getBuyOrders().size(), 0);
    ASSERT_EQ(GetCore().getSellOrders().size(), 0);

    ASSERT_EQ(GetCore().getBuyHeap().size(), 0);
    ASSERT_EQ(GetCore().getSellHeap().size(), 0);
}

// 2 клиента постепенно пришли к сделке
TEST_F(TestFixture, Match_6) {
    GetCore().createOrder("1", 1, 63, true);
    GetCore().createOrder("1", 1, 64, true);
    GetCore().createOrder("1", 1, 65, true);
    GetCore().createOrder("1", 1, 66, true);
    GetCore().createOrder("1", 1, 67, true);

    GetCore().createOrder("2", 1, 71, false);
    GetCore().createOrder("2", 1, 70, false);
    GetCore().createOrder("2", 1, 69, false);
    GetCore().createOrder("2", 1, 68, false);
    GetCore().createOrder("2", 1, 67, false);

    GetCore().match();

    ASSERT_EQ(GetCore().getBuyOrders().size(), 4);
    ASSERT_EQ(GetCore().getSellOrders().size(), 4);

    ASSERT_EQ(GetCore().getBuyHeap().size(), 4);
    ASSERT_EQ(GetCore().getSellHeap().size(), 4);
}
