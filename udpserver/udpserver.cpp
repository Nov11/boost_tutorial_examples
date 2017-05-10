// udpserver.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <boost/asio.hpp>
#include <iostream>
#include <boost\thread.hpp>
#include <boost\bind.hpp>
using namespace boost::asio::ip;
using namespace boost::system;


boost::asio::io_service io;

void handle_connections()
{
	char buff[1024];
	udp::socket sock(io, udp::endpoint(udp::v4(), 60000));
	while (true)
	{
		udp::endpoint ep;
		size_t bytes = sock.receive_from(boost::asio::buffer(buff), ep);
		std::string msg(buff, bytes);
		sock.send_to(boost::asio::buffer(msg), ep);
	}
}
int main()
{
	handle_connections();

    return 0;
}

