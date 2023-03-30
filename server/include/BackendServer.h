#pragma once

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <cstdlib>
#include <iostream>

#include "Common.hpp"
#include "Session.h"
#include "json.hpp"

using boost::asio::ip::tcp;

class Server {
   public:
    Server(boost::asio::io_service &io_service);

    void handle_accept(Session *new_session,
                       const boost::system::error_code &error);

   private:
    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
};