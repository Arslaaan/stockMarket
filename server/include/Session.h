#pragma once

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "json.hpp"
#include "Common.hpp"
#include "Core.h"

using boost::asio::ip::tcp;

class Session
{
public:
    Session(boost::asio::io_service& io_service);

    tcp::socket& socket();

    void start();

    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred);

    void handle_write(const boost::system::error_code& error);

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};