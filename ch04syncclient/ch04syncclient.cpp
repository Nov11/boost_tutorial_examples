#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <string>
#include <iostream>
#include <vector>
using namespace boost::asio;

class talk_to_server {
public:
	talk_to_server(boost::asio::io_service& service, const std::string& username)
		:sock_(service),
		started_(true), username_(username) {

	}

	void loop() {
		write("login " + username_ + "\n");
		read_answer();
		while (started_) {
			write_request();
			read_answer();
			boost::this_thread::sleep(boost::posix_time::millisec(rand() % 7000));
		}
	}

	void write_request() {
		write("ping\n");
	}

	void read_answer() {
		already_read_ = 0;
		already_read_ = read(sock_, buffer(buff_), boost::bind(&talk_to_server::read_complete, this, buff_, _1, _2));
		process_msg();
	}
	void process_msg() {
		std::string msg(buff_, already_read_);
		std::cout << username_ << " server sent me : " << msg << std::endl;
		if (msg.find("login ") == 0)on_login();
		else if (msg.find("ping") == 0)on_ping(msg);
		else if (msg.find("clients ") == 0)on_clients(msg);
		else std::cerr << "invalid msg" << msg << std::endl;
	}

	size_t read_complete(char* buf, const boost::system::error_code& err, size_t bytes)
	{
		if (err) { return 0; }
		bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
		return found ? 0 : 1;
	}

	void on_login() { do_ask_clients(); }
	void on_ping(const std::string & msg) {
		std::istringstream in(msg);
		std::string  answer;
		in >> answer >> answer;
		if (answer == "client_list_changed") {
			do_ask_clients();
		}
	}
	void on_clients(const std::string& msg) {
		std::string clients = msg.substr(8);
		std::cout << username_ << ", new client list:" << clients;
	}
	void do_ask_clients() {
		write("ask_clients\n");
		read_answer();
	}
	void write(const std::string & msg) { sock_.write_some(buffer(msg)); }
	void connect(ip::tcp::endpoint& ep) {
		sock_.connect(ep);
	}
	//std::string username()const { return username_; }
private:
	boost::asio::ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	size_t already_read_ = 0;
	char buff_[max_msg];
	bool started_;
	std::string username_;
};


ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 60000);
void run_close(io_service& io, const std::string& client_name) {
	talk_to_server client(io, client_name);
	try {
		client.connect(ep);
		client.loop();
	}
	catch (boost::system::system_error & ec) {
		std::cout << "client terminated: " << ec.what() << std::endl;
	}
}

int main()
{
	boost::asio::io_service io;
	std::vector<std::string> names{ "c1" , "c2", "c3" , "c4" , "c5", "c6", "c7", "c8" };
	boost::thread_group threads;
	for (auto&item : names) {
		threads.create_thread(boost::bind(&run_close, boost::ref(io), item));
	}
	//for (auto item : names) {
	//	run_close(io, item);
	//}
	threads.join_all();
}