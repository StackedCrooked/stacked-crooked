#include "ScopedMuter.h"
#include "ScopedRedirect.h"
#include <iostream>
#include <fstream>


void testMute()
{
    std::cout << "Before mute" << std::endl;
    {
        ScopedMuter mute(std::cout);
        std::cout << "During mute." << std::endl;
    }
    std::cout << "After mute" << std::endl;
}


void testRedirect()
{
    std::ofstream filestream("redirect_cout");
    std::cout << "Before redirect." << std::endl;
    {
        ScopedRedirect redirect(std::cout, filestream);
        std::cout << "During redirect." << std::endl;
    }
    std::cout << "After redirect." << std::endl;
}



int main()
{
    testMute();
    testRedirect();
    return 0;
}
