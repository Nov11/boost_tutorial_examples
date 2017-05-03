// syncDaytimeServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "boost/asio/ip/tcp.hpp"

using boost::asio::ip::tcp;

std::string make_daytime_string() {
	using namespace std;
	time_t now = time(0);
	vector<char> buf(80);
	auto ret = ctime_s(buf.data(), buf.size(), &now);
	if (ret == 0) {
		return string(buf.begin(), buf.end());
	}
	return "";
}

int main()
{
	boost::asio::io_service io;
	tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 13));
	while (true) {
		tcp::socket socket(io);
		acceptor.accept(socket);
		std::string msg = make_daytime_string();

		boost::system::error_code ignore_error;
		boost::asio::write(socket, boost::asio::buffer(msg), ignore_error);
	}
}

