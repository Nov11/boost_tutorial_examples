#include <boost/noncopyable.hpp>
#include <vector>
#include <iostream>
#include <algorithm>


class A : boost::noncopyable{};
class B : A {};

int main()
{
	//A a;
	//B b;
	std::vector<int> v{ 1,2,3,4,5,6 };
	int i = 4;
	auto ret = std::find_if(v.begin(), v.end(), [&](int param)->bool {return param == i; });
	std::cout << (ret != v.end()) << std::endl;
	std::cout << *ret << std::endl;
}