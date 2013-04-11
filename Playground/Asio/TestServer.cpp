#include "MessageServer.h"


int main()
{
    MessageProtocol::MessageServer server(9999, [](const MessageProtocol::MessageSession & session, const std::string & req) {
        return std::to_string(reinterpret_cast<long>(&session)) + " " + req;
    });
}
