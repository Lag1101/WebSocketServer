// WebSocketServer.cpp : Defines the entry point for the console application.
//
#include <bitset>
#include <iostream>


#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Session.h"

using boost::asio::ip::tcp;


class server
{
public:
	server(boost::asio::io_service& io_service, unsigned short port)
		: io_service_(io_service),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		Session* new_session = new Session(io_service_);
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
	}

	void handle_accept(Session* new_session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_session->handshake();
			new_session = new Session(io_service_);
			acceptor_.async_accept(new_session->socket(),
				boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
		}
		else
		{
			delete new_session;
		}
	}

private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
};

int main()
{
	try
	{
		boost::asio::io_service io_service;

		server s(io_service, 3001);

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}


