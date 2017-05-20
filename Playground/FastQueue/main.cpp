#include "Classic.h"


int main()
{
    std::cout << "Started" << std::endl;
    Queue q;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    for (auto i = 0u; i != 100u * 1000u; ++i)
    {
        q.push(Packet{1});
    }
    q.stop();
    std::cout << "Stopping" << std::endl;
}
