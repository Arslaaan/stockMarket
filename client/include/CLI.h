#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>

#include "Client.h"

namespace CLI {
enum Commands { CONNECT, BUY, SELL, HELP, EXIT };
struct HelpInfo {
    std::string description;
    std::string usage;
};
static std::unordered_map<std::string, Commands> const commandTable = {
    {"connect", CONNECT},
    {"buy", BUY},
    {"sell", SELL},
    {"help", HELP},
    {"exit", EXIT}};

static std::unordered_map<Commands, HelpInfo> const helpTable = {
    {CONNECT, {"Log in to server with name ${USER}", "connect -u user"}},
    {BUY,
     {"Buy ${AMOUNT_1[type int]} USD where each costs ${AMOUNT_2[type double]} "
      "RUB.",
      "buy 5 70.50"}},
    {SELL,
     {"Sell ${AMOUNT_1[type int]} USD where each costs ${AMOUNT_2[type "
      "double]} RUB.",
      "sell 5 70.50"}},
    {HELP, {"Shows help for every command", "help"}},
    {EXIT, {"Exit client", "exit"}},
};

void Run(std::unique_ptr<Client>& client, const std::string& cmd) {
    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char>> tokens(cmd, sep);

    auto token = tokens.begin();
    boost::algorithm::to_lower(*token);

    switch (commandTable.at(*token)) {
        case CONNECT: {
            if (checkSize(CONNECT, tokens)) {
                if (*(++token) == "-u") {
                    client->processRegistration(*(++token));
                    break;
                }
            }
            ShowHelp(helpTable.at(CONNECT));
            break;
        }
        case BUY: {
            //todo
            break;
        }
        case SELL: {
            //todo
            break;
        }
        case HELP: {
            if (checkSize(HELP, tokens)) {
                ShowHelp();
            }
            break;
        }
        case EXIT: {
            if (checkSize(EXIT, tokens)) {
                exit(0);
            }
            break;
        }
        default: {
            std::cout << "Unknown menu option\n" << std::endl;
        }
    }
}

void ShowHelp() {
    std::cout << "Here you can see all commands:" << std::endl;
    for (auto& it : helpTable) {
        ShowHelp(it.second);
    }
}
void ShowHelp(const HelpInfo& helpInfo) {
    std::cout << "\t" << helpInfo.description << "Usage: " << helpInfo.usage
              << std::endl;
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
};  // namespace CLI