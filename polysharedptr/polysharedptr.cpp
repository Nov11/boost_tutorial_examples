// polysharedptr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <memory>
#include <iostream>
class B {
public:
	// Definition of class B
	virtual void someBMethod() {
		// Make a burger
		std::cout << "from class b" << std::endl;
	};
};

class C : public B {
public:
	// Definition of class C
	void someBMethod() {
		// Make a pizza
		std::cout << "from class c" << std::endl;
	};
};

class A {
public:
	A(const std::shared_ptr<B>& pB) : pMember(pB) {}
	std::shared_ptr<B> pMember;
};

int main() {
	A SomeA(std::shared_ptr<B>(new C()));
	SomeA.pMember->someBMethod(); // someBMethod from C is being executed.
}
