#include "MessageClient.h"
#include <thread>


int main()
{
    MessageProtocol::MessageClient client("127.0.0.1", 9999);
    for (;;)
    {
        MessageProtocol::get_io_service().poll();
    }
}
