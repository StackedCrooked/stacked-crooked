#include "ScopedMuter.h"
#include <iostream>


int main()
{
	std::cout << "Before mute" << std::endl;
	{
		ScopedMuter mute(std::cout);
		std::cout << "During mute." << std::endl;
	}
	std::cout << "After mute" << std::endl;
	return 0;
}
