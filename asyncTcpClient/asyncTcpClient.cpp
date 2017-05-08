#include <iostream>
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace	boost::asio::ip;
using namespace boost::system;

class talk_to_server : public boost::enable_shared_from_this<talk_to_server>, boost::noncopyable
{
public:
	static boost::shared_ptr<talk_to_server> start(tcp::endpoint ep, const std::string& message, boost::asio::io_service& io)
	{
		boost::shared_ptr<talk_to_server> result(new talk_to_server(message, io));
		result->start(ep);
		return result;
	}
	void stop()
	{
		if (!started_) { return; }
		started_ = true;
		sock_.close();
	}
	bool started() { return started_; }
private:

	typedef talk_to_server self_type;
	talk_to_server(const std::string & message, boost::asio::io_service& service)
		:sock_(service), started_(true), message_(message)
	{

	}
	void on_connect(const error_code& ec)
	{
		if (ec) { stop(); }
		else { do_write(message_ + "\n"); }
	}

	void start(tcp::endpoint ep)
	{
		sock_.async_connect(ep, boost::bind(&talk_to_server::on_connect, shared_from_this(), _1));
	}
	void on_read(const error_code& ec, size_t bytes)
	{
		if (ec) { stop(); }
		std::string s(read_buffer_, bytes == 0 ? 0 : bytes - 1);
		std::cout << "server eched our " << message_ << " :" << (s == message_ ? "OK" : "FAIL") << std::endl;
	}
	void do_read()
	{
		async_read(sock_, boost::asio::buffer(read_buffer_), boost::bind(&talk_to_server::read_complete, read_buffer_, _1, _2), boost::bind(&talk_to_server::on_read, shared_from_this(), _1, _2));
	}
	void on_write(const error_code& ec, size_t bytes)
	{
		do_read();
	}
	void do_write(const std::string& msg)
	{
		if (!started()) { return; }
		std::copy(msg.begin(), msg.end(), write_buffer_);
		sock_.async_write_some(boost::asio::buffer(write_buffer_, msg.size()), boost::bind(&talk_to_server::on_write, shared_from_this(), _1, _2));
	}
	static size_t read_complete(char* buf, const error_code& err, size_t bytes)
	{
		if (err) { return 0; }
		bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
		return found ? 0 : 1;
	}
	tcp::socket sock_;
	static const int max_msg = 1024;
	char read_buffer_[1024];
	char write_buffer_[1024];
	bool started_;
	std::string message_;
};

int main()
{
	boost::asio::io_service io;
	tcp::endpoint ep(address::from_string("127.0.0.1"), 60000);
	char * message[] = { "John says hi", "so does James", "Lucy just got home", "Boost ASIO", 0 };
	for (char** iter = message; *iter; iter++) {
		talk_to_server::start(ep, *iter/* message[0]*/, io);
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
	io.run();
	int i;
	std::cin >> i;
}