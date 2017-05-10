#include <boost/noncopyable.hpp>


class A : boost::noncopyable{};
class B : A {};

int main()
{
	A a;
	B b;
}