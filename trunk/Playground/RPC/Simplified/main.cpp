#include "RPC.h"


#ifdef RPC_SERVER


class TestServer
{
public:
    TestServer(short port) :
        mUDPServer(port, boost::bind(&TestServer::handleRequest, _1))
    {
    }

    std::string handleRequest(const std::string & req)
    {
        return req;
    }

private:
    UDPServer mUDPServer;
};


void runServer(short port)
{
    std::cout << "Listening to port " << port << std::endl;
    TestServer(port);
}


#endif



#ifdef RPC_CLIENT


void runClient(const std::string & host, short port)
{
    UDPClient client(host, port);

}


#endif


int main()
{
    short port = 9001;
    #if RPC_SERVER
    runServer(port);
    #elif RPC_CLIENT
    runClient("127.0.0.1", port);
    #else
    #error "Invalid build config."
    #endif
}
