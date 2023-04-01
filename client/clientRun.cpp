#include <boost/asio.hpp>
#include <iostream>

#include "Common.hpp"
#include "json.hpp"
#include "Client.h"

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1",
                                   std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);
        
        auto client = std::make_unique<Client>(io_service);
        
        client->getSocket().connect(*iterator);
        // Мы предполагаем, что для идентификации пользователя будет
        // использоваться ID. Тут мы "регистрируем" пользователя - отправляем на
        // сервер имя, а сервер возвращает нам ID. Этот ID далее используется
        // при отправке запросов.
        client->processRegistration();

        std::cout << "Now you can enter your commands" << std::endl;

        while (true) {
            std::string cmd;
            std::cin >> cmd;
            switch (cmd) {
                case 1: {
                    // Для примера того, как может выглядить взаимодействие с
                    // сервером реализован один единственный метод - Hello. Этот
                    // метод получает от сервера приветствие с именем клиента,
                    // отправляя серверу id, полученный при регистрации.
                    SendMessage(s, my_id, Requests::Hello, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2: {
                    exit(0);
                    break;
                }
                default: {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}