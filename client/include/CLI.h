#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>

#include "Client.h"

struct HelpInfo {
    std::string description;
    std::string usage;
};

class CLI {
   public:
    enum Commands { CONNECT, BUY, SELL, HELP, EXIT };
    CLI(std::unique_ptr<Client>& client_) : client(client_){};

    void Run(const std::string& cmd) {
        boost::char_separator<char> sep(" ");
        boost::tokenizer<boost::char_separator<char>> tokens(cmd, sep);

        auto token = tokens.begin();
        if (token == tokens.end()) {
            return;
        }
        // boost::algorithm::to_lower(*token);

        switch (commandTable.at(*token)) {
            case CONNECT: {
                if (checkSize(CONNECT, tokens)) {
                    if (*(++token) == "-u") {
                        client->connect(*(++token));
                        break;
                    }
                }
                ShowHelp(helpTable.at(CONNECT));
                break;
            }
            case BUY: {
                SendTransaction(BUY, tokens);
                break;
            }
            case SELL: {
                SendTransaction(SELL, tokens);
                break;
            }
            case HELP: {
                if (checkSize(HELP, tokens)) {
                    ShowHelp();
                }
                ShowHelp(helpTable.at(HELP));
                break;
            }
            case EXIT: {
                if (checkSize(EXIT, tokens)) {
                    exit(0);
                }
                ShowHelp(helpTable.at(EXIT));
                break;
            }
            default: {
                std::cout << "Unknown menu option" << std::endl;
            }
        }
    };
    void ShowHelp() {
        std::cout << "Here you can see all commands:" << std::endl;
        for (auto& it : helpTable) {
            ShowHelp(it.second);
        }
    }

   private:
    const std::unordered_map<std::string, Commands> commandTable = {
        {"connect", CONNECT},
        {"buy", BUY},
        {"sell", SELL},
        {"help", HELP},
        {"exit", EXIT}};

    const std::unordered_map<Commands, HelpInfo> helpTable = {
        {CONNECT,
         {"Connect: log in to server with name ${USER}", "connect -u user"}},
        {BUY,
         {"Buy: buy ${AMOUNT_INT} USD where each costs ${AMOUNT_DOUBLE} RUB",
          "buy 5 70.50"}},
        {SELL,
         {"Sell: sell ${AMOUNT_INT} USD where each costs ${AMOUNT_DOUBLE} RUB",
          "sell 5 70.50"}},
        {HELP, {"Help: Shows help for every command", "help"}},
        {EXIT, {"Exit: Exit client", "exit"}},
    };

    void SendTransaction(
        Commands cmd,
        const boost::tokenizer<boost::char_separator<char>>& tokens) {
        if (checkSize(cmd, tokens)) {
            auto token = tokens.begin();
            try {
                std::string amountString = *++token;
                std::string costString = *++token;
                int amount = std::stoi(amountString);
                double cost = std::stod(costString);
                if (cmd == BUY) {
                    client->transaction(Requests::Buy, amountString,
                                        costString);
                } else {
                    client->transaction(Requests::Sell, amountString,
                                        costString);
                }
            } catch (const std::exception& e) {
                std::cout << "Wrong types of numbers" << std::endl;
                ShowHelp(helpTable.at(cmd));
            }
        }
    }

    void ShowHelp(const HelpInfo& helpInfo) {
        std::cout << "\t" << helpInfo.description << "." << std::endl;
        std::cout << "\t\tUsage: " << helpInfo.usage << std::endl;
    }
    bool checkSize(
        Commands cmd,
        const boost::tokenizer<boost::char_separator<char>>& tokensActual) {
        boost::char_separator<char> sep(" ");
        boost::tokenizer<boost::char_separator<char>> tokensExpected(
            helpTable.at(cmd).usage, sep);
        return std::distance(tokensExpected.begin(), tokensExpected.end()) ==
               std::distance(tokensActual.begin(), tokensActual.end());
    }

    std::unique_ptr<Client>& client;
};  // namespace CLI