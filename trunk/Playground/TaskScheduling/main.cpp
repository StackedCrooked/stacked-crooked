#include "ThreadSupport.h"
#include <iostream>


int main()
{
    using ThreadSupport::Scheduler;

    // create scheduler on the stack
    Scheduler s;
    
    // schedule a number of taskes with timeouts
    for (int i = 1; i < 1024 * 1024; i = 2 * i)
    {
        s.schedule([=]{ std::cout << "i: " << i << std::endl; }, i);
    }
    
    // sleep for one second to allow some of the tasks to run
    sleep(1);
    
    // End of scope => Scheduler will be destroyed => scheduled threads are cancelled!
    std::cout << "End of scope!" << std::endl;
}
