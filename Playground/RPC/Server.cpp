#include "Networking.h"
#include "Commands.h"
#include "Stopwatch.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>


using namespace boost::tuples;


typedef boost::shared_ptr<Stopwatch> StopwatchPtr;
std::vector<StopwatchPtr> mStopwatches;


typedef tuple<std::string, std::string> NameArg;


std::string HandleRequest(const std::string & inRequest)
{
    NameArg nameAndArg = deserialize<NameArg>(inRequest);
    const std::string & name = nameAndArg.get<0>();
    const std::string & arg = nameAndArg.get<1>();
    std::cout << "HandleRequest: name: " << name << std::endl;
    if (name == CreateStopwatch::CommandName())
    {
        mStopwatches.push_back(boost::make_shared<Stopwatch>());
        CreateStopwatch::Ret ret(RemotePtr(mStopwatches.back().get()), arg);
        return serialize(ret);
    }
    return "Unknown command: " + inRequest;
}


int main()
{
    UDPServer server(9001);
    server.run(boost::bind(&HandleRequest, _1));
}
