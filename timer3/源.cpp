#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void print(const boost::system::error_code& e, boost::asio::deadline_timer* t, int* count) {
	if (*count < 5) {
		std::cout << *count << std::endl;
		++(*count);
		t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
		t->async_wait(boost::bind(print, boost::asio::placeholders::error, t, count));
	}
}

class printer {
public:
	printer(boost::asio::io_service& io)
		: count_(0)
		, timer_(io, boost::posix_time::seconds(1))
	{
		timer_.async_wait(boost::bind(&printer::print, this));
	}
	~printer() {
		std::cout << "count:" << count_ << std::endl;
	}

	void print() {
		if (count_ < 5) {
			count_++;
			std::cout << "in handler : " << count_ << std::endl;

			timer_.expires_at((timer_.expires_at() + boost::posix_time::seconds(1)));
			//timer_.async_wait(boost::bind(&printer::print, this));
			timer_.async_wait([this]() {
				print();
			});
		}
	}
private:
	boost::asio::deadline_timer timer_;
	int count_;
};
//timer 3
//int main() {
//	int count = 0;
//	boost::asio::io_service io;
//	boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
//	t.async_wait(boost::bind(print, boost::asio::placeholders::error, &t, &count));
//	io.run();
//	std::cout << " done " << std::endl;
//}

//timer 4
int main() {
	boost::asio::io_service io;
	printer p(io);
	io.run();
	std::cout << " done " << std::endl;
}