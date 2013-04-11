#include "MessageClient.h"
#include <thread>




int main()
{
    
    std::thread t(boost::bind(&boost::asio::io_service::run, &MessageProtocol::get_io_service()));
    
    std::thread c1([&t](){
        MessageProtocol::MessageClient client("127.0.0.1", 9999);   
        for (int i = 0; i != 10; ++i)
        {
            client.send(std::to_string(i), [i](const std::string & str) {
                std::cout << "c1 received " << i << ": " << str << std::endl;
            });
            std::cout << "c1 sent " << i << std::endl;
        }
        t.join();
    });
    std::thread c2([&t](){
        MessageProtocol::MessageClient client("127.0.0.1", 9999);   
        for (int i = 0; i != 10; ++i)
        {
            client.send(std::to_string(i), [i](const std::string & str) {
                std::cout << "c2 received " << i << ": " << str << std::endl;
            });
            std::cout << "c2 sent " << i << std::endl;
        }     
        t.join();   
    });
    std::thread c3([&t](){
        MessageProtocol::MessageClient client("127.0.0.1", 9999);   
        for (int i = 0; i != 10; ++i)
        {
            client.send(std::to_string(i), [i](const std::string & str) {
                std::cout << "c3 received " << i << ": " << str << std::endl;
            });
            std::cout << "c3 sent " << i << std::endl;
        }   
        t.join();     
    });
    
    c1.join();
    c2.join();
    c3.join();
    return 0;
}
