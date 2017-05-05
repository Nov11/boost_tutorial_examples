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
	cout << ss.str() << endl;
}

void run_dispatch_and_post() {
	for (int i = 0; i < 10; i += 2) {
		io.dispatch(boost::bind(func, i));
		io.post(boost::bind(func, i + 1));
	}
}

int main(){}