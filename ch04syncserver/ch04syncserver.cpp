#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <iostream>
#include <algorithm>
using namespace boost::asio;


using boost::posix_time::ptime;

class talk_to_client;
using client_ptr = boost::shared_ptr<talk_to_client>;
using array = std::vector<client_ptr>;
array clients;
boost::recursive_mutex cs;
boost::asio::io_service io;

class talk_to_client :public boost::enable_shared_from_this<talk_to_client> {
public:
	talk_to_client(io_service& io) :sock_(io) {}
	std::string username()const { return username_; }
	void answer_to_client() {
		try {
			read_request();
			process_request();
		}
		catch (boost::system::system_error&) {
			stop();
		}
		if (timed_out()) {
			stop();
		}
	}

	void set_clients_changed() {
		clients_changed_ = true;
	}
	ip::tcp::socket& sock() {
		return sock_;
	}
	bool timed_out()const {
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		long long ms = (now - last_ping_).total_milliseconds();
		return ms > 5000;
	}

	void stop() {
		boost::system::error_code err;
		sock_.close(err);
	}

	void read_request() {
		if (sock_.available()) {
			already_read_ += sock_.read_some(buffer(buff_ + already_read_, max_msg - already_read_));
		}
	}

	void process_request() {
		bool found_enter = std::find(buff_, buff_ + already_read_, '\n') < buff_ + already_read_;
		if (!found_enter) { return; }
		last_ping_ = boost::posix_time::microsec_clock::local_time();
		size_t pos = std::find(buff_, buff_ + already_read_, '\n') - buff_;
		std::string msg(buff_, pos);
		std::copy(buff_ + already_read_, buff_ + max_msg, buff_);
		already_read_ -= pos + 1;
		if (msg.find("login ") == 0)on_login(msg);
		else if (msg.find("ping") == 0)on_ping();
		else if (msg.find("ask_clients") == 0)on_clients();
		else std::cerr << "invalid msg" << msg << std::endl;
	}

	void on_login(const std::string &msg) {
		std::istringstream in(msg);
		in >> username_ >> username_;
		write("login ok\n");
		//update_clients_changed();
		clients_changed_ = true;
	}
	void on_ping() {
		write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
		clients_changed_ = false;
	}
	void on_clients() {
		std::string msg;
		{
			boost::recursive_mutex::scoped_lock lk(cs);
			for (array::const_iterator b = clients.begin(), e = clients.end(); b != e; ++b) {
				msg += (*b)->username() + " ";
			}
		}
		write("clients " + msg + "\n");
	}
	void write(const std::string & msg) { sock_.write_some(buffer(msg)); }
	ip::tcp::socket& socket() { return sock_; }
private:
	boost::asio::ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	size_t already_read_ = 0;
	char buff_[max_msg];
	bool started_;
	std::string username_;
	bool clients_changed_;
	ptime last_ping_;

};


void accept_thread() {
	ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 60000));
	while (true) {
		client_ptr new_(new talk_to_client(io));
		acceptor.accept(new_->socket());
		boost::recursive_mutex::scoped_lock lk(cs);
		clients.push_back(new_);
	}
}

void handler_clients_thread() {
	while (true) {
		boost::this_thread::sleep(boost::posix_time::millisec(1));
		boost::recursive_mutex::scoped_lock lk(cs);
		array clients_tmp;
		for (auto b = clients.begin(); b != clients.end(); b++) {
			(*b)->answer_to_client();
			//clients.erase(std::remove_if(clients.begin(), clients.end(), boost::bind(&talk_to_client::timed_out, _1)));
			if (!(*b)->timed_out()) { clients_tmp.push_back(*b); }
		}
		swap(clients, clients_tmp);
	}
}

int main()
{
	boost::thread_group threads;
	threads.create_thread(accept_thread);
	threads.create_thread(handler_clients_thread);
	threads.join_all();
}