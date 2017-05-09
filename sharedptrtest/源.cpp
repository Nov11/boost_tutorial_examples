#include <boost/shared_ptr.hpp>
#include <vector>
#include <iostream>
#include "../boost_1_62_0/boost/smart_ptr/enable_shared_from_this.hpp"


using namespace boost;

class A {

};
class B :public enable_shared_from_this<B> , public A{};
class C :public B{};
//if we place c in the end, this works just fine
class D : public C, public enable_shared_from_this<D> {
public:
	D(int i){}
	boost::shared_ptr<D> fun() { return boost::enable_shared_from_this<D>::shared_from_this(); }
};

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
	//std::cout << (v[0].get() == ptr.get()) << std::endl;

	std::cout << "======================" << std::endl;
	boost::shared_ptr<D> stmptr(new D(12));
	D* pd = stmptr.get();
	pd->fun();
}