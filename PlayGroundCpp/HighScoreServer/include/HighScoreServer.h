#ifndef HIGHSCORESERVER_H_INCLUDED
#define HIGHSCORESERVER_H_INCLUDED


#include "Poco/Net/HTTPServer.h"
#include "Poco/Util/ServerApplication.h"


namespace HSServer
{

    class HighScoreServer : public Poco::Util::ServerApplication
    {
    protected:
	    int main(const std::vector<std::string>& args);
    };

} // HighScoreServer


#endif // HIGHSCORESERVER_H_INCLUDED
