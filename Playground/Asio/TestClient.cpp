#include "Client.h"
#include <thread>


int main(int argc, char * argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        Client c(io_service, iterator);

        std::thread t(bind(&io_service::run, &io_service));

        char line[Message::max_body_length + 1];
        while (std::cin.getline(line, Message::max_body_length + 1))
        {
            using namespace std; // For strlen and memcpy.
            Message msg;
            msg.body_length(strlen(line));
            memcpy(msg.body(), line, msg.body_length());
            msg.encode_header();
            c.write(msg);
        }

        c.close();
        t.join();
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
