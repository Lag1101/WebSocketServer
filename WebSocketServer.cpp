// WebSocketServer.cpp : Defines the entry point for the console application.
//
#include <bitset>
#include <iostream>
#include <deque>


#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Session.h"

#include "boost_http_server/boost_http_server_entry.hpp"

using boost::asio::ip::tcp;


class server
{
public:

	void accept()
	{
		std::deque<Session*>::iterator new_session = sessions.insert(sessions.end(), new Session(io_service_));

		acceptor_.async_accept((*new_session)->socket(),
			boost::bind(&server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
	}

	void handle_accept(std::deque<Session*>::iterator new_session, 
		const boost::system::error_code& error)
	{
		if (!error)
			(*new_session)->handshake();
		else
		{
			delete (*new_session);
			sessions.erase(new_session);
		}
 		accept();
	}


	server(boost::asio::io_service& io_service, unsigned short port)
		: io_service_(io_service),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		accept();
	}
	~server()
	{
		std::for_each(sessions.begin(), sessions.end(), [](Session *& session){
			delete session;
		});
	}

private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	std::deque<Session*> sessions;
};

int main(int argc, char* argv[])
{
	try
	{
		entry(argc, argv);
//		boost::asio::io_service io_service;
//
//		server s(io_service, 3001);
//
//		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}


