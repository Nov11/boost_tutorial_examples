#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
	//char* str = "hello";
	//vector<char> v(str, str + strlen(str));
	//char* cptr = &v[0];
	//string s = *(string*)cptr;
	//cout << s << endl;
	string a = "abc";
	string b;
	string c("123456789012345678901234567890");
	cout << sizeof(a) << " " << sizeof(b) << " " << sizeof(c) << endl;
	vector<char> v;
	v.resize(sizeof(std::string));
	char* cptr = &v[0];
	*(string*)cptr = c;
	string output(v.begin(), v.end());
	cout << output << endl;
}