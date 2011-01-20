#include "Crooked/ScopedMuter.h"
#include <iostream>


int main()
{
	std::cout << "Before mute" << std::endl;
	{
		Crooked::ScopedMuter mute(std::cout);
		std::cout << "During mute." << std::endl;
	}
	std::cout << "After mute" << std::endl;
	return 0;
}
