#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include "../common/БъЭЗ.h"
#include "boost/enable_shared_from_this.hpp"
using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class udp_server {
public:
	udp_server(boost::asio::io_service& io)
		:socket_(io, udp::endpoint(udp::v4(), 13))
	{
		start_accept();
	}
private:
	boost::array<char, 1> rbuf_;
	udp::socket socket_;
	udp::endpoint remote_;
	std::string msg_;
	void start_accept() {
		socket_.async_receive_from(boost::asio::buffer(rbuf_), remote_, boost::bind(&udp_server::handle_receive, this, boost::asio::placeholders::error));
	}
	void handle_receive(const boost::system::error_code& error) {
		if (!error || error == boost::asio::error::message_size) {
			//boost::shared_ptr<std::string> msg(new std::string(make_daytime_string()));
			msg_ = make_daytime_string();
			socket_.async_send_to(boost::asio::buffer(msg_), remote_, boost::bind(&udp_server::handle_send, this));
			start_accept();
		}
	}
	void handle_send() {
		std::cout << "udp server : sent: " << msg_ << std::endl;
	}
};

class tcp_connection
	:public boost::enable_shared_from_this<tcp_connection> {
public:
	typedef boost::shared_ptr<tcp_connection> pointer;
	static pointer create(boost::asio::io_service& io) {
		return boost::shared_ptr<tcp_connection>(new tcp_connection(io));
	}
	tcp::socket& socket() {
		return socket_;
	}
	void bussinessLogic() {
		msg_ = make_daytime_string();
		socket_.async_send(boost::asio::buffer(msg_), boost::bind(&tcp_connection::handle_send, shared_from_this()));
	}

private:
	tcp::socket socket_;
	std::string msg_;
	tcp_connection(boost::asio::io_service& io)
		:socket_(io)
	{
	}

	void handle_send() {
		std::cout << "tcp server : msg sent to client" << std::endl;
	}
};

class tcp_server {
public:
	tcp_server(boost::asio::io_service& io)
		:acceptor_(io, tcp::endpoint(tcp::v4(), 13))
	{
		start_accept();
	}
private:
	tcp::acceptor acceptor_;
	void start_accept() {
		tcp_connection::pointer ptr = tcp_connection::create(acceptor_.get_io_service());
		acceptor_.async_accept(ptr->socket(), boost::bind(&tcp_server::handle_receive, this, boost::asio::placeholders::error, ptr));
	}
	void handle_receive(const boost::system::error_code& error, tcp_connection::pointer ptr) {
		if (!error) {
			ptr->bussinessLogic();
		}
		start_accept();
	}
};

int main() {
	try {
		boost::asio::io_service io;
		udp_server udpsvr(io);
		tcp_server tcpsvr(io);
		io.run();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}