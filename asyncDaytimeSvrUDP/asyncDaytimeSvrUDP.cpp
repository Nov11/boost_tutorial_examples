#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "../common/БъЭЗ.h"
using boost::asio::ip::udp;

class udp_server {
public:
	udp_server(boost::asio::io_service& io)
		:socket_(io, udp::endpoint(udp::v4(), 13)) {
		start_receive();
	}

private:
	udp::socket socket_;
	udp::endpoint remote_;
	boost::array<char, 1> rbuf_;
	void start_receive() {
		socket_.async_receive_from(boost::asio::buffer(rbuf_)
			, remote_
			, boost::bind(&udp_server::handle_receive
				, this
				, boost::asio::placeholders::error
				, boost::asio::placeholders::bytes_transferred)
		);
	}
	//hint: const is needed here before param 'error', or there will be a compile error about type conversion
	void handle_receive(const boost::system::error_code& error, std::size_t) {
		if (!error || error == boost::asio::error::message_size) {
			boost::shared_ptr<std::string> msg(new std::string(make_daytime_string()));
			//msg is safe to use in buffer's ctor.
			//Because function obj holds its ref until handle_send. Till calling handle_seng, buffer's already been used.
			socket_.async_send_to(boost::asio::buffer(*msg), remote_, boost::bind(&udp_server::handle_send, this, msg, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			start_receive();
		}
	}
	void handle_send(boost::shared_ptr <std::string>, const boost::system::error_code & , std::size_t){}
};


int main() {
	try {
		boost::asio::io_service io;
		udp_server server(io);
		io.run();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}