#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "../common/БъЭЗ.h"

using boost::asio::ip::udp;
int main() {
	try {
		boost::asio::io_service io;
		udp::socket socket(io, udp::endpoint(udp::v4(), 13));
		while (true) {
			boost::array<char, 1> rbuf;
			udp::endpoint peer;
			boost::system::error_code error;
			socket.receive_from(boost::asio::buffer(rbuf), peer, 0, error);
			if (error && error != boost::asio::error::message_size)
				throw boost::system::system_error(error);
			std::string msg = make_daytime_string();
			
			socket.send_to(boost::asio::buffer(msg), peer, 0, error);
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}