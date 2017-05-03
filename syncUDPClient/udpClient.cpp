// asyncDaytimeSvrUDP.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "boost/asio/ip/udp.hpp"

using boost::asio::ip::udp;


int main()
{
	try {
		boost::asio::io_service io;
		udp::resolver re(io);
		udp::resolver::query q(udp::v4(), "localhost", "daytime");
		udp::endpoint endpoint = *re.resolve(q);
		udp::socket socket(io);
		socket.open(udp::v4());//hint:��д����Ļ���������ļ������Ч���쳣
		boost::array<char, 1> send_buf = { { 0 } };
		socket.send_to(boost::asio::buffer(send_buf), endpoint);
		boost::array<char, 128> rbuf;
		udp::endpoint sender_ep;
		size_t len = socket.receive_from(boost::asio::buffer(rbuf), sender_ep);
		std::cout.write(rbuf.data(), len);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

