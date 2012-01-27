#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "RPC/Command.h"
#include "RPC/RemoteObjects.h"


namespace RPC {


typedef std::string URL;


struct CreateStopwatchArgs
{
public:
    CreateStopwatchArgs() {}

    CreateStopwatchArgs(const RemoteServer & inServer, const std::string & inName) :
        mServer(inServer),
        mName(inName)
    {
    }

    const RemoteServer & server() const { return mServer; }

    const std::string & name() const { return mName; }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & mServer & mName;
    }

private:
    RemoteServer mServer;
    std::string mName;
};


struct CreateStopwatch : public ConcreteCommand<RemoteStopwatch(CreateStopwatchArgs)>
{
    typedef ConcreteCommand<RemoteStopwatch(CreateStopwatchArgs)> Super;

    static const char * CommandName() { return "CreateStopwatch"; }

    CreateStopwatch(const RemoteServer & inServer, const std::string & inName) :
        Super(CommandName(), CreateStopwatchArgs(inServer, inName))
    {
    }
};


struct StartStopwatch : public ConcreteCommand<bool(RemoteStopwatch)>
{
    typedef ConcreteCommand<bool(RemoteStopwatch)> Super;

    static const char * CommandName() { return "StartStopwatch"; }

    StartStopwatch(const RemoteStopwatch & inStopwatch) :
        Super(CommandName(), inStopwatch)
    {
    }
};


} // namespace RPC


#endif // RPC_COMMANDS_H
