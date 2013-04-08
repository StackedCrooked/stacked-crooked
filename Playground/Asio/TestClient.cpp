#include "MessageClient.h"
#include <thread>


int main()
{
    MessageClient client("127.0.0.1", 9999);

    std::thread t(boost::bind(&boost::asio::io_service::run, &get_io_service()));

    std::string line;
    while (std::cin >> line)
    {
        std::future<std::string> result = client.write(line);
        std::cout << "Result: " << result.get() << std::endl;
    }

    client.close();
    t.join();
    return 0;
}
