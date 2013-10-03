#include "RequestHandler.h"
#include "ContentType.h"
#include "Renderer.h"
#include "Utils.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Util/Application.h"
#include "Poco/StreamCopier.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <sstream>


using namespace Poco::Data;


namespace HSServer
{

    Poco::Logger & GetLogger()
    {
        return Poco::Util::Application::instance().logger();
    }


    MissingArgumentException::MissingArgumentException(const std::string & inMessage) :
        std::runtime_error(inMessage)
    {
    }


    RequestHandler::RequestHandler(ResourceId inResourceId, Method inMethod, ContentType inContentType) :
        mSession(SessionFactory::instance().create("SQLite", "HighScores.db")),
        mResourceId(inResourceId),
        mMethod(inMethod),
        mContentType(inContentType)
    {
        // Create the table if it doesn't already exist
        static bool fFirstTime = [&]() -> bool {
            mSession << "CREATE TABLE IF NOT EXISTS HighScores("
                     << "Id INTEGER PRIMARY KEY, "
                     << "Timestamp INTEGER, "
                     << "Name VARCHAR(20), "
                     << "Score INTEGER(5))", now;
            return true;
        }();
        (void)fFirstTime;
    }


    void RequestHandler::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                       Poco::Net::HTTPServerResponse& outResponse)
    {

        GetLogger().information("Request from " + inRequest.clientAddress().toString());
        GetLogger().information("Request Accept header: " + inRequest.get("Accept"));
        outResponse.setChunkedTransferEncoding(true);
        outResponse.setContentType(ToString(mContentType));

        try
        {
            generateResponse(inRequest, outResponse);
        }
        catch (const std::exception & inException)
        {
            GetLogger().error(inException.what());
        }
    }


} // namespace HSServer
