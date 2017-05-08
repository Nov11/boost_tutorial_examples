#include <boost/shared_ptr.hpp>
#include <vector>
#include <iostream>

int main()
{
	//int * ptr = new int(12);
	//boost::shared_ptr<int> s1(ptr);
	//boost::shared_ptr<int> s2(ptr);
	//std::cout << (s1 == s2) << std::endl;
	std::vector < boost::shared_ptr<int>> v;
	auto ptr = boost::shared_ptr<int>(new int(23));
	v.push_back(ptr);
	auto ptr2 = ptr;
	v.push_back(ptr2);
	std::cout << (v[0] == ptr) << std::endl;
	std::cout << (v[1] == ptr) << std::endl;
}