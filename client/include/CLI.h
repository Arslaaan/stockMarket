#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>
#include <memory>

#include "Client.h"

struct HelpInfo {
    std::string description;
    std::string usage;
};

class CLI {
   public:
    enum Commands { CONNECT, BUY, SELL, HELP, BALANCE, HISTORY, ACTIVE, EXIT };
    CLI(std::unique_ptr<Client>& client_);

    void Run(const std::string& cmd);
    void ShowHelp();

   private:
    const std::map<std::string, Commands> commandTable = {
        {"connect", CONNECT}, {"buy", BUY},         {"sell", SELL},
        {"help", HELP},       {"balance", BALANCE}, {"history", HISTORY},
        {"active", ACTIVE},   {"exit", EXIT}};

    const std::map<Commands, HelpInfo> helpTable = {
        {CONNECT,
         {"Connect: log in to server with name ${USER}", "connect -u user"}},
        {BUY,
         {"Buy: buy ${AMOUNT_INT} USD where each costs ${AMOUNT_DOUBLE} RUB",
          "buy 5 70.50"}},
        {SELL,
         {"Sell: sell ${AMOUNT_INT} USD where each costs ${AMOUNT_DOUBLE} RUB",
          "sell 5 70.50"}},
        {HELP, {"Help: Shows help for every command", "help"}},
        {BALANCE, {"Balance: Shows current balance", "balance"}},
        {HISTORY,
         {"History: Shows history of completed (partially or not) orders",
          "history"}},
        {ACTIVE, {"Active: Shows info about active orders", "active"}},
        {EXIT, {"Exit: Exit client", "exit"}},
    };

    void OpenOrder(Commands cmd,
                   const boost::tokenizer<boost::char_separator<char>>& tokens);

    void ShowHelp(const HelpInfo& helpInfo);
    bool checkSize(
        Commands cmd,
        const boost::tokenizer<boost::char_separator<char>>& tokensActual);

    std::unique_ptr<Client>& client;
};