#include <iostream>
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace	boost::asio::ip;
using namespace boost::system;

class AsyncTcpServerConnection : public boost::enable_shared_from_this<AsyncTcpServerConnection>, boost::noncopyable
{
public:
	typedef boost::shared_ptr<AsyncTcpServerConnection> ConnPTR;
	static boost::shared_ptr<AsyncTcpServerConnection> make_new_server_connection(boost::asio::io_service& io)
	{
		boost::shared_ptr<AsyncTcpServerConnection> result(new AsyncTcpServerConnection(io));
		return result;
	}
	void stop() {
		if (!started_) { return; }
		started_ = false;
		sock_.close();
	}
	void run_server()
	{
		started_ = true;
		do_read();
	}
	tcp::socket& socket() { return sock_; }
private:
	boost::asio::io_service& io_;
	tcp::socket sock_;
	static const int buffSize = 1024;
	char read_buffer_[buffSize];
	char write_buffer_[buffSize];
	bool started_;

	AsyncTcpServerConnection(boost::asio::io_service& io)
		:io_(io), sock_(io),started_(false){}

	void on_read(const error_code& ec, size_t bytes)
	{
		if (ec) { stop(); }
		do_write(std::string(read_buffer_, bytes));
	}
	void do_read()
	{
		async_read(sock_, boost::asio::buffer(read_buffer_), boost::bind(&AsyncTcpServerConnection::read_complete, read_buffer_, _1, _2), boost::bind(&AsyncTcpServerConnection::on_read, shared_from_this(), _1, _2));
	}
	void on_write(const error_code& ec, size_t bytes)
	{
		do_read();
	}
	void do_write(const std::string& msg)
	{
		if (!started_) { return; }
		std::copy(msg.begin(), msg.end(), write_buffer_);
		async_write(sock_, boost::asio::buffer(write_buffer_, msg.size()), boost::bind(&AsyncTcpServerConnection::on_write, shared_from_this(), _1, _2));
	}
	static size_t read_complete(char* buf, const error_code& err, size_t bytes)
	{
		if (err) { return 0; }
		bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
		return found ? 0 : 1;
	}
};
	boost::asio::io_service io;
	tcp::endpoint ep(tcp::v4(), 60000);
	tcp::acceptor acceptor(io, ep);
void on_accept(const error_code& ec, AsyncTcpServerConnection::ConnPTR ptr)
{
	if (ec) { exit(1); }
	ptr->run_server();

	auto srv = AsyncTcpServerConnection::make_new_server_connection(io);
	acceptor.async_accept(srv->socket(), boost::bind(&on_accept, _1, srv));
}
int main()
{
	auto srv = AsyncTcpServerConnection::make_new_server_connection(io);
	acceptor.async_accept(srv->socket(), boost::bind(&on_accept, _1, srv));
	io.run();
}