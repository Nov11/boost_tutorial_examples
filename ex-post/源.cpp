#include <iostream>
#include <boost/asio.hpp>
#include "boost/thread.hpp"
#include <string>
using namespace boost::asio;
using namespace std;

io_service io;

void func(int i) {
	stringstream ss;
	ss << "func called from ";
	ss << boost::this_thread::get_id();
	ss << " param:";
	ss << to_string(i);
	ss << endl;
	string tmp = ss.str();
	cout.write(tmp.c_str(), tmp.size());
}

void worker_thread() {
	io.run();
}

int main() {
	io_service::strand sone(io), stwo(io);
	for (int i = 0; i < 5; i++) {
		io.post(sone.wrap(boost::bind(func, i)));
	}

	for (int i = 5; i < 10; i++) {
		io.post(stwo.wrap(boost::bind(func, i)));
	}

	boost::thread_group threads;
	for (int i = 0; i < 4; i++) {
		threads.create_thread(worker_thread);
	}
	boost::this_thread::sleep(boost::posix_time::millisec(500));
	threads.join_all();
}
