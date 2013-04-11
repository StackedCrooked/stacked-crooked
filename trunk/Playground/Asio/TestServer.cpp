#include "MessageServer.h"


int main()
{
    MessageProtocol::MessageServer server(9999, [](MessageProtocol::AbstractMessageSession & session, const std::string & str) {
        return std::to_string(reinterpret_cast<long>(&session)) + ": " + str;        
    });
}
