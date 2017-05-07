#include <iostream>
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace	boost::asio::ip;
using namespace boost::system;

boost::asio::ip::tcp::endpoint ep(address::from_string("127.0.0.1"), 60000);
boost::asio::io_service service;

size_t read_complete(char* buf, const error_code& err, size_t bytes)
{
	if (err) { return 0; }
	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	return found ? 0 : 1;
}

void sync_echo(std::string msg) {
	msg += "\n";
	tcp::socket sock(service);
	sock.connect(ep);
	sock.write_some(boost::asio::buffer(msg));
	char buf[1024] = {};
	error_code ec;
	size_t bytes = read(sock, 
		boost::asio::buffer(buf), 
		boost::bind(read_complete, buf, _1, _2), ec);
	if (ec) {
		system_error se(ec);
		std::cerr << se.what() << std::endl;
		return;
	}
	std::string copy(buf, bytes == 0 ? 0 : bytes - 1);
	msg = msg.substr(0, msg.size() - 1);
	std::cout << "server echoed our " << msg << " :" << (copy == msg ? "OK" : "FAIL") << std::endl;
	sock.close();
}

int main()
{
	try {
	char * message[] = { "John says hi", "so does James", "Lucy just got home", "Boost ASIO", 0 };
	boost::thread_group threads;
	for (char** iter = message; *iter; iter++) {
		threads.create_thread(boost::bind(sync_echo, *iter));
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
	threads.join_all();
		//sync_echo(message[0]);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

}