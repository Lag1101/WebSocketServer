//
// Created by luckybug on 22.03.15.
//

#ifndef _WEBSOCKETSERVER_SESSION_H_
#define _WEBSOCKETSERVER_SESSION_H_

#include <boost/asio.hpp>

#include "Frame.h"


using boost::asio::ip::tcp;

class Session
{

public:
    Session(boost::asio::io_service& io_service);

    tcp::socket& socket();
    void read();
    void handshake();

    std::string exstractKey();

    std::string calcAccept(const std::string & key);

    void parseFrame(const std::vector<unsigned char> & data);

    void handle_accept(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_write(const boost::system::error_code& error);

private:
    tcp::socket socket_;
    std::vector<unsigned char> data_;
};

#endif //_WEBSOCKETSERVER_SESSION_H_
