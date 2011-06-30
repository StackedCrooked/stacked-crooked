#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

struct Foo {
	Foo(std::size_t length, char value) : data(length, value) { }

	Foo(const Foo & rhs) : data(rhs.data) { std::cout << "*** COPY ***" << std::endl; }

	Foo & operator= (Foo rhs) {
		std::cout << "*** ASSIGNMENT ***" << std::endl;
		std::swap(data, rhs.data);
		return *this;
	}

	~Foo() { }

	std::vector<char> data;
};

Foo TestRVO() { return Foo(512, 'r'); }

Foo TestNamedRVO() { Foo result(512, 'n'); return result; }

void PassByValue(Foo inFoo)
{
    std::cout << inFoo.data.size() << std::endl;
}

int main()
{
	//std::cout << "\nTest RVO: " << std::endl;
	//Foo rvo = TestRVO();

	//std::cout << "\nTest named RVO: " << std::endl;
	//Foo named_rvo = TestNamedRVO();

	//std::cout << "\nTest PassByValue: " << std::endl;
	//Foo foo(512, 'a');
	//PassByValue(foo);
    PassByValue(Foo(512, 'a'));

	//std::cout << "\nTest assignment: " << std::endl;
	//Foo f(512, 'f');
	//Foo g(512, 'g');
	//f = g;


    //std::vector<char> vec(1024, 'c');
    //std::size_t totalSize = 0;
    //for (std::size_t idx = 0; idx < 1024 * 1024; ++idx)
    //{
        //std::vector<char> copy = vec;
        //totalSize += copy.size();
    //}
    //std::cout << "Total size: " << totalSize << std::endl;
}

