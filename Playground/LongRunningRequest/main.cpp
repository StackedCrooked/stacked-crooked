#include "Networking.h"
#include "Bridge.h"





int main()
{
    boost::thread listener_thread([&](){
        Server server;
        UDPServer(9100, [&](const Request & req) {
            return Response(server.handle(req));
        });
    });

    boost::thread worker_thread([](){
        UDPClient client("localhost", 9100);
        std::string req = client.send("listen");

        // send the request itself as reply
        UDPSender("localhost", 9100).send(join("finished", req));
    });

    worker_thread.join();
    listener_thread.join();
}
