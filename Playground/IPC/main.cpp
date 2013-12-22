#include <atomic>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>



std::atomic<bool> gQuit{false};

void run()
{
    system("rm -f command");
    unsigned n = 0;
    bool b = false;
    std::cout << "Try me: echo pwd >command" << std::endl;
    std::cout << "To quit: echo q >command" << std::endl;

    while (!gQuit)
    {
        usleep(2000000);
        try 
        {
            std::cout << n << '\r';
            std::ifstream is("command");
            std::string cmd;
            while (is >> cmd)
            {
                if (!b) std::cout << std::endl;
                b = true;
                if (cmd == "q")
                {
                    system("rm -f command");
                    return;
                }
                std::cout << "Executing " << cmd << ":" << std::endl;
                system(cmd.c_str());
            }
            system("rm -f command");

        }
        catch (const std::exception& e)
        {
            std::cout << "caught: " << e.what() << std::endl;
        }
    }
}




int main()
{
    std::thread(&run).join();
}
