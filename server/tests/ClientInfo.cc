#include <gtest/gtest.h>

#include "Core.h"

TEST(ClientInfo, testAddToBalance_1) {
    const std::string& a = "qwe";
    const std::string& b = "qwe";
    const std::string& c = "qwe";
    ClientInfo cl(a, b, c);

    ASSERT_EQ(cl.getBalance(Currency::RUR), 0);
    ASSERT_EQ(cl.getBalance(Currency::USD), 0);
}

TEST(ClientInfo, testAddToBalance_2) {
    ClientInfo cl("qwe", "qwe", "qwe");

    cl.addToBalance(Currency::RUR, 50.1);
    cl.addToBalance(Currency::USD, 100);

    ASSERT_EQ(cl.getBalance(Currency::RUR), 50.1);
    ASSERT_EQ(cl.getBalance(Currency::USD), 100);
}

TEST(ClientInfo, takeFromBalance_1) {
    ClientInfo cl("qwe", "qwe", "qwe");

    cl.takeFromBalance(Currency::RUR, 50.1);
    cl.takeFromBalance(Currency::USD, 100);

    ASSERT_EQ(cl.getBalance(Currency::RUR), -50.1);
    ASSERT_EQ(cl.getBalance(Currency::USD), -100);
}

TEST(ClientInfo, historyUpdate_1) {
    ClientInfo cl("qwe", "qwe", "qwe");

    cl.historyUpdate("qweqwe");
    cl.historyUpdate("asdasd");

    ASSERT_EQ(cl.getHistory().at(0), "qweqwe");
    ASSERT_EQ(cl.getHistory().at(1), "asdasd");
}

TEST(ClientInfo, addRemoveTest) {
    ClientInfo cl("qwe", "qwe", "qwe");

    cl.addActiveOrder("qweqwe");
    cl.addActiveOrder("asdasd");
    cl.removeOrder("qweqwe");

    ASSERT_EQ(cl.getActiveOrders().at(0), "asdasd");
    ASSERT_EQ(cl.getActiveOrders().size(), 1);
}