// WebSocketServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <bitset>
#include <iostream>

using boost::asio::ip::tcp;

std::vector<unsigned char> calcSHA1(const std::string & str)
{
	boost::uuids::detail::sha1 sha1;

	sha1.process_bytes(str.c_str(), str.size());

	unsigned int digest[5];
	sha1.get_digest(digest);

	unsigned char hash[20];
	for(int i = 0; i < 5; ++i)
	{
		const char* tmp = reinterpret_cast<char*>(digest);
		hash[i*4] = tmp[i*4+3];
		hash[i*4+1] = tmp[i*4+2];
		hash[i*4+2] = tmp[i*4+1];
		hash[i*4+3] = tmp[i*4];
	}

	return std::vector<unsigned char>(hash, hash+20);
}

std::vector<unsigned char> encryptBase64(const void * data, size_t size)
{
	using namespace boost::archive::iterators;
	typedef insert_linebreaks<base64_from_binary<transform_width<const char *,6,8>>, 72> base64_text;	

	std::vector<unsigned char> res;
	std::copy(
		base64_text((char*)data),
		base64_text((char*)data + size),
		std::back_inserter(res)
		);
	res.push_back('=');

	return res;
}

void misc()
{
	std::string str = "some text";

	std::vector<unsigned char> sha1 = calcSHA1(str);

	std::cout <<  std::hex;
	std::copy((short*)&sha1[0], (short*)&sha1[0] + sha1.size() / 2, std::ostream_iterator<unsigned int>(std::cout, " "));

	std::vector<unsigned char> b64 = encryptBase64(&sha1[0], sha1.size());
	//std::copy(b64.begin(), b64.end(), std::ostream_iterator<unsigned int>(std::cout, ""));
}

class Session
{
public:
	Session(boost::asio::io_service& io_service)
		: socket_(io_service), data_(1024), hanshaked(false)
	{
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void read()
	{
		socket_.async_read_some(boost::asio::buffer(data_, data_.size()),
			boost::bind(&Session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	std::string exstractKey()
	{
		static const int keySize = 24;
		static const std::string keyStr = "Sec-WebSocket-Key: ";

		std::string str(data_.cbegin(), data_.cend());

		int s = str.find(keyStr) + keyStr.size();

		return std::string(str.cbegin() + s, str.cbegin() + s + keySize);
	}

	std::string calcAccept(const std::string & key)
	{
		static const std::string ext = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		std::string extKey = key + ext;

		std::vector<unsigned char> sha1 = calcSHA1(extKey);

		std::vector<unsigned char> res = encryptBase64(&sha1[0], sha1.size());

		return std::string(res.cbegin(), res.cend());
	}

	void parseFrame(const std::vector<unsigned char> & data)
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

	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (!error)
		{
			std::cout<< std::string(data_.cbegin(), data_.cbegin() + bytes_transferred) << std::endl;

			if(hanshaked) {
				parseFrame(data_);
			} else {
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

				hanshaked = true;
			}				
		}
		else
		{
			delete this;
		}
	}

	void handle_write(const boost::system::error_code& error)
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

private:
	tcp::socket socket_;
	std::vector<unsigned char> data_;
	bool hanshaked;
};

class server
{
public:
	server(boost::asio::io_service& io_service, short port)
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
			new_session->read();
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

int _tmain(int argc, _TCHAR* argv[])
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


