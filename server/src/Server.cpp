#include "Server.h"

#include <boost/asio.hpp>

#include "Session.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service &io_service)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
    std::cout << "Server started! Listen " << port << " port" << std::endl;

    Session *new_session = new Session(io_service_);
    acceptor_.async_accept(
        new_session->socket(),
        boost::bind(&Server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
}

void Server::handle_accept(Session *new_session,
                           const boost::system::error_code &error) {
    if (error) {
        delete new_session;
        return;
    }
    
    new_session->start();
    new_session = new Session(io_service_);
    acceptor_.async_accept(
        new_session->socket(),
        boost::bind(&Server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
}
