#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <boost/enable_shared_from_this.hpp>
using namespace boost::asio;


class talk_to_server
	:public boost::enable_shared_from_this<talk_to_server>
	, boost::noncopyable
{
public:
	talk_to_server(const std::string & username, boost::asio::io_service& io)
		:username_(username), io_(io), sock_(io), timer_(io)
	{

	}

	void start(ip::tcp::endpoint ep) {
		sock_.async_connect(ep, boost::bind(&talk_to_server::on_connect, shared_from_this(), _1));
	}
	static boost::shared_ptr<talk_to_server> start(ip::tcp::endpoint ep, const std::string& username, io_service& io) {
		boost::shared_ptr<talk_to_server> new_(new talk_to_server(username, io));
		new_ -> start(ep);
		return new_;
	}
	void stop() {
		if (!started_) { return; }
		started_ = false;
		sock_.close();
	}

	void on_connect(const boost::system::error_code& err) {
		if (!err) { do_write("login " + username_ + "\n"); }
		else {
			
			boost::system::system_error sr{ err };
			std::cout << sr.what() << std::endl;
			
			stop(); }
	}

	void on_read(const boost::system::error_code & err, size_t bytes) {
		if (err) { stop(); }
		if (!started_) { return; }
		
		std::string msg(read_buff_, bytes);
		if (msg.find("login ") == 0) { on_login(); }
		else if (msg.find("ping") == 0) { on_ping(msg); }
		else if (msg.find("clients ") == 0) { on_clients(msg); }
	}
	void on_login() {
		do_ask_clients();
	}

	void on_ping(const std::string & msg) {
		std::istringstream in(msg);
		std::string  answer;
		in >> answer >> answer;
		if (answer == "client_list_changed") {
			do_ask_clients();
		}
		else {
			postpone_ping();
		}
		std::cout << "on ping :" << msg << std::endl;
	}
	void on_clients(const std::string& msg) {
		std::string clients = msg.substr(8);
		std::cout << username_ << ", new client list:" << clients;
		postpone_ping();
	}
	void do_ask_clients() {
		do_write("ask_clients\n");
	}
	void do_ping() {
		do_write("ping\n");
	}
	void postpone_ping() {
		timer_.expires_from_now(boost::posix_time::millisec(rand() % 7000));
		timer_.async_wait(boost::bind(&talk_to_server::do_ping, shared_from_this()));
	}
	void do_write(const std::string & msg) {
		if (!started_) { return; }
		std::copy(msg.begin(), msg.end(), write_buff_);
		sock_.async_write_some(buffer(write_buff_, msg.size()), boost::bind(&talk_to_server::on_write, shared_from_this(), _1, _2));
	}
	void on_write(const boost::system::error_code& err, size_t bytes) {
		do_read();
	}
	void do_read() {
		async_read(sock_, buffer(read_buff_), boost::bind(&talk_to_server::read_complete, read_buff_, _1, _2), boost::bind(&talk_to_server::on_read, shared_from_this(), _1, _2));
	}
	static size_t read_complete(char* buf, const boost::system::error_code& err, size_t bytes)
	{
		if (err) { return 0; }
		bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
		return found ? 0 : 1;
	}
private:
	std::string username_;
	boost::asio::io_service& io_;
	ip::tcp::socket sock_;
	bool started_ = true;
	char read_buff_[1024];
	char write_buff_[1024];
	deadline_timer timer_;
};


int main()
{
	boost::asio::io_service io;
	std::vector<std::string> names{ "c1" , "c2", "c3" , "c4" , "c5", "c6", "c7", "c8" };
	boost::shared_ptr<talk_to_server> ptr = talk_to_server::start(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 60000), "c1", io);
	io.run();
}