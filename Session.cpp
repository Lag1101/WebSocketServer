//
// Created by luckybug on 22.03.15.
//

#include <iterator>
#include <iostream>

#include <boost/bind.hpp>

#include "Session.h"
#include "utils/Utils.h"

Session::Session(boost::asio::io_service& io_service)
: socket_(io_service), data_(1024)
{
}

tcp::socket& Session::socket()
{
    return socket_;
}

void Session::read()
{
    socket_.async_read_some(boost::asio::buffer(data_, data_.size()),
            boost::bind(&Session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Session::handshake() {
    socket_.async_read_some(boost::asio::buffer(data_, data_.size()),
            boost::bind(&Session::handle_accept, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}


std::string Session::exstractKey()
{
    static const int keySize = 24;
    static const std::string keyStr = "Sec-WebSocket-Key: ";

    std::string str(data_.cbegin(), data_.cend());

    size_t s = str.find(keyStr) + keyStr.size();

    return std::string(str.cbegin() + s, str.cbegin() + s + keySize);
}

std::string Session::calcAccept(const std::string & key)
{
    static const std::string ext = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    std::string extKey = key + ext;

    std::vector<unsigned char> sha1 = Utils::Crypt::calcSHA1(extKey);

    std::vector<unsigned char> res = Utils::Crypt::encryptBase64(&sha1[0], sha1.size());

    return std::string(res.cbegin(), res.cend());
}

void Session::parseFrame(const std::vector<unsigned char> & data)
{
    Frame frame = Frame::decode(data);

    std::cout << "Received :" << frame << std::endl;

    Frame ansFrame = Frame::createTextFrame("Hello from server with Love");
    auto ans = ansFrame.getRaw();
    boost::asio::async_write(socket_,
            boost::asio::buffer(&ans[0], ans.size()),
            boost::bind(&Session::handle_write, this,
                    boost::asio::placeholders::error));


    std::cout << "Sent :" << ansFrame << std::endl;
}

void Session::handle_accept(const boost::system::error_code& error,
        size_t bytes_transferred)
{
    if (!error)
    {
        std::cout<< std::string(data_.cbegin(), data_.cbegin() + bytes_transferred) << std::endl;

        std::string key = exstractKey();
        std::string accept =  calcAccept(key);

        const std::string handshake = std::string() +
                "HTTP/1.1 101 Switching Protocols\n" +
                "Upgrade: websocket\n" +
                "Connection: Upgrade\n" +
                "Sec-WebSocket-Accept: " + accept + "\n\n";

        std::cout << handshake << std::endl;

        boost::asio::async_write(socket_,
                boost::asio::buffer(handshake.c_str(), handshake.size()),
                boost::bind(&Session::handle_write, this,
                        boost::asio::placeholders::error));
    }
    else
    {
        delete this;
    }
}

void Session::handle_read(const boost::system::error_code &error, size_t bytes_transferred) {
    if (!error)
    {
        parseFrame(data_);
    }
    else
    {
        delete this;
    }
}

void Session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        read();
    }
    else
    {
        delete this;
    }
}
