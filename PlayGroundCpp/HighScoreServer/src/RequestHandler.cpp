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

    
    RequestHandler::RequestHandler(ResourceId inResourceId, RequestMethod inRequestMethod, ContentType inContentType) :
        mSession(SessionFactory::instance().create("SQLite", "HighScores.db")),
        mResourceId(inResourceId),
        mRequestMethod(inRequestMethod),
        mContentType(inContentType)
    {
        // Create the table if it doesn't already exist
        static bool fFirstTime(true);
        if (fFirstTime)
        {
            mSession << "CREATE TABLE IF NOT EXISTS HighScores("
                     << "Id INTEGER PRIMARY KEY, "
                     << "Timestamp INTEGER, "
                     << "Name VARCHAR(20), "
                     << "Score INTEGER(5))", now;
            fFirstTime = false;
        }
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


    HTMLErrorResponse::HTMLErrorResponse(const std::string & inErrorMessage) :
        RequestHandler(ResourceId_End, RequestMethod_Get, ContentType_TextHTML),
        mErrorMessage(inErrorMessage)
    {
    }


    void HTMLErrorResponse::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {
        std::string html = MakeHTMLDocument("Error", MakeHTML("p", mErrorMessage, HTMLFormatting_OneLiner));
        outResponse.setContentLength(html.size());
        outResponse.send() << html;
    }


    void GetHighScorePostForm::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {        
        std::string body;
        ReadEntireFile("html/add.html", body);
        outResponse.setContentLength(body.size());
        outResponse.send() << body;
    }


    void GetHighScoreDeleteForm::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {        
        std::string body;
        ReadEntireFile("html/delete.html", body);
        outResponse.setContentLength(body.size());
        outResponse.send() << body;
    }


    void PostHightScore::generateResponse(Poco::Net::HTTPServerRequest& inRequest,
                                          Poco::Net::HTTPServerResponse& outResponse)
    {
        std::string requestBody;
        inRequest.stream() >> requestBody;

        Args args;
        GetArgs(requestBody, args);
        
        std::string name = URIDecode(GetArg(args, "name"));
        const std::string & score = GetArg(args, "score");

        Statement insert(mSession);
        insert << "INSERT INTO HighScores VALUES(NULL, strftime('%s', 'now'), ?, ?)", use(name), use(score);
        insert.execute();

        // Return an URL instead of a HTML page.
        // This is because the client is the JavaScript application in this case.
        std::string body = cResourceLocations[GetResourceId()];
        outResponse.setContentLength(body.size());
        outResponse.send() << body;
    }


    void DeleteHighScore::generateResponse(Poco::Net::HTTPServerRequest& inRequest,
                                           Poco::Net::HTTPServerResponse& outResponse)
    {
        std::string requestBody;
        inRequest.stream() >> requestBody;

        GetLogger().information("Request body is: " + requestBody);

        std::string sql = Poco::replace<std::string>("DELETE FROM HighScores WHERE {{args}}",
                                                     "{{args}}",
                                                     Args2String(GetArgs(requestBody)));

        GetLogger().information("SQL statement is: " + sql);

        Statement insert(mSession);
        insert << sql;
        insert.execute();

        // Return a message indicating success.
        std::string body = "Succesfully performed the following SQL statement: " + sql;
        outResponse.setContentLength(body.size());
        outResponse.send() << body;
    }


} // namespace HSServer
