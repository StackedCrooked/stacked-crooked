#include "MessageClient.h"
#include <thread>


int main()
{
    MessageClient client("127.0.0.1", 9999);

    std::thread t(boost::bind(&boost::asio::io_service::run, &get_io_service()));

    std::string line;
    while (std::cin >> line)
    {
        client.write(line);
    }

    client.close();
    t.join();
    return 0;
}
