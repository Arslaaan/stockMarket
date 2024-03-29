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
        std::cout << "Incorrect command, please see help" << std::endl;
        ShowHelp(helpTable.at(command));
        return;
    }
    switch (command) {
        case LOGIN:
        case REGISTER: {
            if (*(++token) == "-u") {
                std::string name = *++token;
                if (*++token == "-p") {
                    std::string pass = *++token;
                    if (command == LOGIN) {
                        client->login(name, pass);
                    } else {
                        client->registration(name, pass);
                    }
                    break;
                }
            }
            std::cout << "Incorrect command, check usage:" << std::endl;
            ShowHelp(helpTable.at(command));
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
        case QUOTE: {
            client->quote();
            break;
        }
        case CANCEL: {
            client->cancel(*++token);
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
        if (cost <= 0 || amount <= 0) {
            throw std::invalid_argument("Numbers must be positive");
        }
        client->openOrder(cmd == BUY ? Requests::Buy : Requests::Sell, amountString, costString);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
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
