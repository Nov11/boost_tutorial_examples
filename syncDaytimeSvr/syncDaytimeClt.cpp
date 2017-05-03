// syncDaytimeSvr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "boost/asio/ip/tcp.hpp"
#include <vcruntime_exception.h>

int main()
{
	try {
	using boost::asio::ip::tcp;
		boost::asio::io_service io;
		tcp::resolver re(io);
		tcp::resolver::query q("localhost", "daytime");
		tcp::resolver::iterator iter = re.resolve(q);
		tcp::socket socket(io);
		boost::asio::connect(socket, iter);
		while (true) {
			boost::array<char, 3> buf;
			boost::system::error_code error;

			size_t len = socket.read_some(boost::asio::buffer(buf), error);
			if (error == boost::asio::error::eof) {
				break;
			}
			else if (error) {
				throw boost::system::system_error(error);
			}
			std::cout.write(buf.data(), len);

		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	
    return 0;
}

