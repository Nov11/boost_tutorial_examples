#include <boost/asio.hpp>
#include <iostream>
#include "boost/thread.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

//other tasks in the same loop thread can influence the time point deadline timer is called
//if a precise timer is needed, consider move this timer to a serperate thread or using busy while loop checking expired timer.

void handler(const boost::system::error_code& error, boost::asio::deadline_timer& timer)
{
	//if (!error) {

	//}
	boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time());
	std::cout << "handler called" << boost::posix_time::to_simple_string(t) << std::endl;
	timer.expires_at(timer.expires_at() + boost::posix_time::seconds(1));
	timer.async_wait(boost::bind(&handler, _1, boost::ref(timer)));
}
int main()
{
	boost::asio::io_service io;
	boost::asio::deadline_timer timer(io, boost::posix_time::seconds(1));
	timer.async_wait(boost::bind( &handler, _1, boost::ref(timer)));
	for (int i = 0; i < 10; i++) {
		io.run_one();
		boost::this_thread::sleep(boost::posix_time::seconds(10));
	}
	std::cout << " done " << std::endl;
	return 0;
}