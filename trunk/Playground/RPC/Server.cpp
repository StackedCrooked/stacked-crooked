#include "Networking.h"
#include "Commands.h"
#include "Stopwatch.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>


typedef boost::shared_ptr<Stopwatch> StopwatchPtr;
std::vector<StopwatchPtr> mStopwatches;


std::string HandleRequest(const std::string & inRequest)
{
    const std::string name = deserialize<Command>(inRequest).className();
    std::cout << "HandleRequest: name: " << name << std::endl;
    if (name == CreateStopwatch::CommandName())
    {
        CreateStopwatch c = deserialize<CreateStopwatch>(inRequest);
        std::string name = c.arg();
        StopwatchPtr stopwatch(new Stopwatch);
        mStopwatches.push_back(stopwatch);
        CreateStopwatch::Ret ret(RemotePtr(stopwatch.get()), name);
        return serialize(ret);
    }
    return "Unknown command: " + inRequest;
}


int main()
{
    UDPServer server(9001);
    server.run(boost::bind(&HandleRequest, _1));
}
