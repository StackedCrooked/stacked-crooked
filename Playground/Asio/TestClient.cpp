#include "MessageClient.h"
#include <thread>


int main()
{
    MessageProtocol::MessageClient client("127.0.0.1", 9999);

    std::thread t(boost::bind(&boost::asio::io_service::run, &MessageProtocol::get_io_service()));

    
    for (int i = 0; i != 10; ++i)
    {
        client.send(std::to_string(i), [i](const std::string & str) {
            std::cout << "received " << i << ": " << str << std::endl;
        });
        std::cout << "sent " << i << std::endl;
    }

    client.close();
    t.join();
    return 0;
}
