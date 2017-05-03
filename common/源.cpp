#include "БъЭЗ.h"
std::string make_daytime_string() {
	using namespace std;
	time_t now = time(0);
	vector<char> buf(80);
	auto ret = ctime_s(buf.data(), buf.size(), &now);
	if (ret == 0) {
		return string(buf.data(), strlen(buf.data()));
	}
	return "";
}