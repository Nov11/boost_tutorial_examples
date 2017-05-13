#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <boost/enable_shared_from_this.hpp>
using namespace boost::asio;


class talk_to_client
	:public boost::enable_shared_from_this<talk_to_client>
	, boost::noncopyable
{
public:
	using ptr = boost::shared_ptr<talk_to_client>;
	talk_to_client(boost::asio::io_service& io);
	void start();
	static boost::shared_ptr<talk_to_client> new_(boost::asio::io_service& io);
	void stop();
	void on_read(const boost::system::error_code& err, size_t bytes);

	void on_login(const std::string &msg);
	void on_ping();
	void on_clients();

	void do_write(const std::string & s);
	void on_write(const boost::system::error_code&err, size_t bytes);
	void do_read();
	static size_t read_complete(char* buf, const boost::system::error_code& err, size_t bytes);
	void on_check_ping();
	void post_check_ping();
	std::string username() const;
	ip::tcp::socket& sock();
private:
	bool started_;
	boost::posix_time::ptime last_ping_;
	ip::tcp::socket sock_;
	char read_buff_[1024];
	char write_buff_[1024];
	std::string username_;
	boost::asio::deadline_timer timer_;
	bool clients_changed_;
};

std::vector<boost::shared_ptr<talk_to_client>> client;
boost::asio::io_service io;
ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 60000));
void handle_accept(talk_to_client::ptr client, const boost::system::error_code& err) {
	client->start();
	talk_to_client::ptr new_client = talk_to_client::new_(io);
	acceptor.async_accept(new_client->sock(), boost::bind(&handle_accept, new_client, _1));
}

int main()
{
	talk_to_client::ptr client = talk_to_client::new_(io);
	acceptor.async_accept(client->sock(), boost::bind(handle_accept, client, _1));
	io.run();
}

inline talk_to_client::talk_to_client(boost::asio::io_service & io) :sock_(io), timer_(io) {}

inline void talk_to_client::start() {
	started_ = true;
	client.push_back(shared_from_this());
	last_ping_ = boost::posix_time::microsec_clock::local_time();
	do_read();
}

inline boost::shared_ptr<talk_to_client> talk_to_client::new_(boost::asio::io_service & io) { return boost::shared_ptr<talk_to_client>(new talk_to_client(io)); }

inline void talk_to_client::stop() {
	if (!started_) { return; }
	started_ = false;
	sock_.close();
	std::remove(client.begin(), client.end(), shared_from_this());
	clients_changed_ = true;
}

inline void talk_to_client::on_read(const boost::system::error_code & err, size_t bytes)
{
	if (err)stop();
	if (!started_) { return; }
	std::string msg(read_buff_, bytes);
	if (msg.find("login ") == 0)on_login(msg);
	else if (msg.find("ping") == 0)on_ping();
	else if (msg.find("ask_clients") == 0)on_clients();
	else std::cerr << "invalid msg" << msg << std::endl;
}

inline void talk_to_client::on_login(const std::string & msg) {
	std::istringstream in(msg);
	in >> username_ >> username_;
	do_write("login ok\n");
	//update_clients_changed();
	clients_changed_ = true;
}

inline void talk_to_client::on_ping() {
	do_write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
	clients_changed_ = false;
}

inline void talk_to_client::on_clients() {
	std::string msg;
	for (auto b = client.begin(); b != client.end(); b++) {
		msg += (*b)->username();
	}
	do_write("clients " + msg + "\n");
}

inline void talk_to_client::do_write(const std::string & s) {
	if (!started_)return;
	std::copy(s.begin(), s.end(), write_buff_);
	sock_.async_write_some(buffer(write_buff_, s.size()), boost::bind(&talk_to_client::on_write, shared_from_this(), _1, _2));
}

inline void talk_to_client::on_write(const boost::system::error_code & err, size_t bytes) { do_read(); }

inline void talk_to_client::do_read() {
	async_read(sock_, buffer(read_buff_), boost::bind(&talk_to_client::read_complete, read_buff_, _1, _2), boost::bind(&talk_to_client::on_read, shared_from_this(), _1, _2));
	post_check_ping();
}

inline size_t talk_to_client::read_complete(char * buf, const boost::system::error_code & err, size_t bytes)
{
	if (err) { return 0; }
	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	return found ? 0 : 1;
}

inline void talk_to_client::on_check_ping() {
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	if ((now - last_ping_).total_milliseconds() > 5000)stop();
	last_ping_ = boost::posix_time::microsec_clock::local_time();
}

inline void talk_to_client::post_check_ping() {
	timer_.expires_from_now(boost::posix_time::millisec(900));
	timer_.async_wait(boost::bind(&talk_to_client::on_check_ping, shared_from_this()));
}

inline std::string talk_to_client::username() const { return username_; }

inline ip::tcp::socket & talk_to_client::sock() { return sock_; }
