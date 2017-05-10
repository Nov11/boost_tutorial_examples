#include <boost/asio.hpp>
#include <iostream>
#include <boost\thread.hpp>
#include <boost\bind.hpp>
using namespace boost::asio::ip;
using namespace boost::system;
udp::endpoint ep(address::from_string("127.0.0.1"), 60000);
boost::asio::io_service service;
void sync_echo(std::string msg)
{
	udp::socket sock(service, udp::endpoint(udp::v4(), 0));
	sock.send_to(boost::asio::buffer(msg), ep);
	udp::endpoint peer;
	char buff[1024];
	size_t bytes = sock.receive_from(boost::asio::buffer(buff), peer);
	std::string copy(buff, bytes);
	std::cout << "server echos back:" << copy << std::endl;
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