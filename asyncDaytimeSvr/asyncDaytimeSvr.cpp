// asyncDaytimeSvr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

std::string make_daytime_string() {
	using namespace std;
	time_t now = time(0);
	vector<char> buf(80);
	auto ret = ctime_s(buf.data(), buf.size(), &now);
	if (ret == 0) {
		return string(buf.data(), strlen(buf.data()));
	}
	return "";
}
class tcp_connection;
class tcp_connection
	:public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;
	static pointer create(boost::asio::io_service& io) {
		return pointer(new tcp_connection(io));
	}
	tcp::socket& socket() {
		return socket_;
	}
	void start() {
		message_ = make_daytime_string();
		boost::asio::async_write(socket_
			, boost::asio::buffer(message_)//messaage_ is member variable, which is valid after this function returns.
			, boost::bind(&tcp_connection::handle_write
				, shared_from_this()
				, boost::asio::placeholders::error
				, boost::asio::placeholders::bytes_transferred));
	}

private:
	tcp::socket socket_;
	std::string message_;
	tcp_connection(boost::asio::io_service& io)
		:socket_(io) {}
	void handle_write(const boost::system::error_code&, size_t) {}
};
class tcp_server {
public:
	tcp_server(boost::asio::io_service& io)
		:acceptor_(io, tcp::endpoint(tcp::v4(), 13)){
		start_accept();
	}
private:
	tcp::acceptor acceptor_;
	void start_accept() {
		tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service());
		acceptor_.async_accept(new_connection->socket(), boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
	}
	void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error) {
		if (!error) {
			new_connection->start();
		}
		start_accept();
	}
};

int main()
{
	boost::asio::io_service io;
	tcp_server server(io);
	io.run();
    return 0;
}

