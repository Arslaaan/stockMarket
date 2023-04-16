#pragma once

#include <boost/functional/hash.hpp>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include "ClientInfo.h"
#include "Common.hpp"
#include "EqualCostOrders.h"
#include "NotificationService.h"
#include "OrderKeeper.h"
#include "TradeHistory.h"
#include "json.hpp"

class Core {
   public:
    void registerNewUser(const std::string& aUserName,
                                const std::string& auth);

    // Запрос ID клиента по имени
    std::optional<std::string> getUserId(const std::string& aUserName);
    // Основная торговая логика.
    // Сводит между собой заявки на покупку и продажу.
    void match();
    // Создает новую заявку
    std::string createOrder(const std::string& clientId, size_t amount,
                            double cost, bool isBuy);

    OrderKeeper& getOrderKeeper();

    const std::priority_queue<double, std::vector<double>, std::less<>>&
    getBuyHeap() const;
    const std::priority_queue<double, std::vector<double>, std::greater<>>&
    getSellHeap() const;
    const std::unordered_map<double, EqualCostOrders>& getBuyOrders() const;
    const std::unordered_map<double, EqualCostOrders>& getSellOrders() const;
    const std::unordered_map<std::string, ClientInfo>& getClientsInfo() const;
    const TradeHistory& getTradeHistory() const;
    const std::string getQuotes() const;
    // Вычисляем хэш от пароля, который используется для аутентификации
    std::string hash(const std::string& text);

   private:
    void trade(const std::unique_ptr<Order>& orderSrc,
               const std::unique_ptr<Order>& orderDst);
    void updateClientInfo(const std::unique_ptr<Order>& order,
                          const std::string& tradeId, bool sellCurrency,
                          double amount);
    void updateOrderPartially(const std::unique_ptr<Order>& order,
                              size_t amountExchanged);
    void updateOrderFull(const std::unique_ptr<Order>& order);

    // <UserName, UserId>
    std::map<std::string, size_t> mUserNames;
    // userId -> clientInfo
    std::unordered_map<std::string, ClientInfo> clientsInfo;
    

    OrderKeeper orderKeeper;
    NotificationService notificationService;
    TradeHistory tradeHistory;
    boost::hash<std::string> hasher;
    double lastBuy; // цена покупки в последней сделке
    double lastSell; // цена продажи в последней сделке

    std::priority_queue<double, std::vector<double>,
                        std::less<>>
        buyHeap;  // max-heap cost-ов заявок на покупку
    std::priority_queue<double, std::vector<double>,
                        std::greater<>>
        sellHeap;  // min-heap cost-ов заявок на продажу
    std::unordered_map<double, EqualCostOrders>
        buyOrders;  // cost -> [uuid_1, ..., uuid_n] для покупок
    std::unordered_map<double, EqualCostOrders>
        sellOrders;  // cost -> [uuid_1, ..., uuid_n] для продажи
};

Core& GetCore();