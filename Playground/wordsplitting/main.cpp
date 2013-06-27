#include <bitset>
#include <iostream>


#define TRACE std::cout << __PRETTY_FUNCTION__ << std::endl;

enum
{
    fin = (1 << 1),
    syn = (1 << 2),
    ack = (1 << 3)
};


int main()
{
	std::cout << std::bitset<8>(fin).to_string() << std::endl;
	std::cout << std::bitset<8>(~fin).to_string() << std::endl;
}
