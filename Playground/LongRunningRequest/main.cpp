#include "Networking.h"
#include "Bridge.h"





int main()
{
    Server http_server;

    UDPServer udp_server(9100, [&](const std::string & req){
        return Response(http_server.handle(req));
    });

}
