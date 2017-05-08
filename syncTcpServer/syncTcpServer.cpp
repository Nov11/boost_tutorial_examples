// syncTcpServer.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace boost::asio::ip;
using namespace boost::system;

boost::asio::io_service io;

size_t read_complete(char* buff, const error_code& err, size_t bytes)
{
	if (err) {
		return 0;
	}
	bool found = std::find(buff, buff + bytes, '\n') < buff + bytes;
	return found ? 0 : 1;
}

void handle_connections() {
	tcp::acceptor acc(io, tcp::endpoint(tcp::v4(), 60000));
	char buff[1024];

	while (true) {
		tcp::socket s(io);
		acc.accept(s);
		size_t bytes = read(s, boost::asio::buffer(buff), boost::bind(read_complete, buff, _1, _2));
		std::string msg(buff);
		s.write_some(boost::asio::buffer(msg));
		s.close();
	}

}

int main()
{
	handle_connections();
}