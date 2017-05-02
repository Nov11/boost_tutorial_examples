#include <iostream>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/asio/impl/io_service.ipp"

//sync timer
//int main() {
//	boost::asio::io_service io;
//	boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
//	t.wait();
//	std::cout << " time out " << std::endl;
//}

//async timer

void print(const boost::system::error_code& ec) {
	std::cout << "print error" << std::endl;
}

void print233() {
	std::cout << "print error" << std::endl;
}

int main() {
	boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
	//t.async_wait(&print);
	t.async_wait(boost::bind(print233));
	io.run();
}