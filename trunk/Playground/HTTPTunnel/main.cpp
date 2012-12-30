#include "Networking.h"
#include "Bridge.h"


/*
ClientServer luistert naar client requests op poort 12340.
WorkerServer luistert naar worker requests op poort 12341.
ResultServer luistert naar worker results  op poort 12342.

- De worker client polt voor request bij de worker server. Dit is een long
  running request die regelmatig vernieuwd wordt.
- De user doet een request bij de client server en wacht op antwoord.
- De client server delegeert de user request naar de worker server.
- De worker server geeft de user request als reponse voor de long running
  worker client request.
- De worker client voert de opdract uit en post het resultaat naar de result
  server.
- De result server geeft het resultaat door aan de client server.
- De client server geeft het resultaat als antwoord op de watchtende client.
 */


enum {
    ClientPort = 12340,
    WorkerPort = 12341,
    ResultPort = 12342
};


struct ClientServer
{
    ClientServer(WorkerServer & worker_server,
                 unsigned short port) :
        mWorkerServer(&worker_server),
        server_(port,
               std::bind(&ClientServer::handle, this, std::placeholders::_1))
    {
    }

    Response handle(const Request & request)
    {
        // De client server delegeert de user request naar de worker server.
        return mWorkerServer->handle(request);
    }

    WorkerServer * mWorkerServer;
    UDPServer server_;
};


int main()
{
    boost::thread listener_thread([&](){
        std::cout << "start listener_thread" << std::endl;
        WorkerServer server;
        UDPServer(9100, [&](const Request & req) {
            std::cout << "Server received request: " << std::endl << req << std::endl;
            return Response(server.handle(req));
        });
    });
    boost::this_thread::sleep_for(boost::chrono::nanoseconds(1000 * 1000 * 1000));

    boost::thread worker_thread([](){
        std::cout << "start worker_thread" << std::endl;
        UDPClient client("localhost", 9101);
        std::string req = client.send("listen");
        std::cout << "worker_thread: got request: " << req << std::endl;
        auto parts = split(req);
        if (parts.at(0) == "flip")
        {
            auto arg = parts.at(1);
            std::reverse(arg.begin(), arg.end());
            UDPSender("localhost", 9100).send(join("finished", join("flip", arg)));
        }
    });
    boost::this_thread::sleep_for(boost::chrono::nanoseconds(1000 * 1000 * 1000));

    boost::thread client_thread([](){
        std::cout << "start client_thread" << std::endl;
        UDPClient client("localhost", 9100);
        std::string req = client.send(join("flip", "abc"));
        std::cout << "client_thread: result of flip: " << req << std::endl;
    });
    boost::this_thread::sleep_for(boost::chrono::nanoseconds(1000 * 1000 * 1000));

    client_thread.join();
    worker_thread.join();
    listener_thread.join();
}
