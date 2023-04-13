#include "CLI.h"

CLI::CLI(std::unique_ptr<Client>& client_) : client(client_){};

void CLI::Run(const std::string& cmd) {
    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char>> tokens(cmd, sep);

    auto token = tokens.begin();
    if (token == tokens.end()) {
        return;
    };
    std::string commandType = *token;
    boost::algorithm::to_lower(commandType);

    if (commandTable.count(commandType) == 0) {
        std::cout << "Unknown menu option" << std::endl;
        return;
    }
    Commands command = commandTable.at(commandType);
    if (!checkSize(command, tokens)) {
        ShowHelp(helpTable.at(command));
    }
    switch (command) {
        case CONNECT: {
            if (*(++token) == "-u") {
                client->connect(*(++token));
            } else {
                ShowHelp(helpTable.at(command));
            }
            break;
        }
        case BUY: {
            OpenOrder(BUY, tokens);
            break;
        }
        case SELL: {
            OpenOrder(SELL, tokens);
            break;
        }
        case HELP: {
            ShowHelp();
            break;
        }
        case BALANCE: {
            client->balance();
            break;
        }
        case HISTORY: {
            client->history();
            break;
        }
        case ACTIVE: {
            client->active();
            break;
        }
        case EXIT: {
            exit(0);
            break;
        }
    }
}

void CLI::ShowHelp() {
    std::cout << "Here you can see all commands:" << std::endl;
    for (auto& it : helpTable) {
        ShowHelp(it.second);
    }
}

void CLI::OpenOrder(
    Commands cmd, const boost::tokenizer<boost::char_separator<char>>& tokens) {
    auto token = tokens.begin();
    try {
        std::string amountString = *++token;
        std::string costString = *++token;
        int amount = std::stoi(amountString);
        double cost = std::stod(costString);
        if (cmd == BUY) {
            client->openOrder(Requests::Buy, amountString, costString);
        } else {
            client->openOrder(Requests::Sell, amountString, costString);
        }
    } catch (const std::exception& e) {
        std::cout << "Wrong types of numbers" << std::endl;
        ShowHelp(helpTable.at(cmd));
    }
}

void CLI::ShowHelp(const HelpInfo& helpInfo) {
    std::cout << "\t" << helpInfo.description << "." << std::endl;
    std::cout << "\t\tUsage: " << helpInfo.usage << std::endl;
}

bool CLI::checkSize(
    Commands cmd,
    const boost::tokenizer<boost::char_separator<char>>& tokensActual) {
    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char>> tokensExpected(
        helpTable.at(cmd).usage, sep);
    return std::distance(tokensExpected.begin(), tokensExpected.end()) ==
           std::distance(tokensActual.begin(), tokensActual.end());
}
