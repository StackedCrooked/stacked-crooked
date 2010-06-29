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

    
    RequestHandler::RequestHandler(RequestMethod inRequestMethod, const std::string & inLocation, ContentType inContentType) :
        mSession(SessionFactory::instance().create("SQLite", "HighScores.db")),
        mRequestMethod(inRequestMethod),
        mLocation(inLocation),
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


    std::string RequestHandler::getSimpleHTML(const std::string & inTitle, const std::string & inBody)
    {
        std::string doc;
        HSServer::ReadEntireFile("html/html-document-template.html", doc);
        return Poco::replace<std::string>(Poco::replace<std::string>(doc, "{{title}}", inTitle), "{{body}}", inBody);
    }


    ContentType RequestHandler::getContentType() const
    {
        return mContentType;
    }


    const std::string & RequestHandler::getPath() const
    {
        return mLocation;
    }

    
    void RequestHandler::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                       Poco::Net::HTTPServerResponse& outResponse)
    {
        
        GetLogger().information("Request from " + inRequest.clientAddress().toString());
        GetLogger().information("Accept: " + inRequest.get("Accept"));
        outResponse.setChunkedTransferEncoding(true);
        outResponse.setContentType(ToString(getContentType()));

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
        RequestHandler(RequestMethod_Get, "", ContentType_TextHTML),
        mErrorMessage(inErrorMessage)
    {
    }


    void HTMLErrorResponse::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {
        std::string html = getSimpleHTML("Error", MakeHTML("p", mErrorMessage, HTMLFormatting_OneLiner));
        outResponse.setContentLength(html.size());
        outResponse.send() << html;
    }
    
    
    RequestHandler * GetHighScoreHTML::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetHighScoreHTML;
    }
    
    
    GetHighScoreHTML::GetHighScoreHTML() :
        RequestHandler(GetRequestMethod(), GetLocation(), GetContentType())
    {
    }


    void GetHighScoreHTML::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {   
        Args args;
        GetArgs(inRequest.getURI(), args);
        std::stringstream whereClause;
        if (!args.empty())
        {
            whereClause << " WHERE ";
            Args::const_iterator it = args.begin(), end = args.end();
            int count = 0;
            for (; it != end; ++it)
            {
                if (count > 0)
                {
                    whereClause << " AND ";
                }
                whereClause << it->first << "='" << it->second << "'";
                count++;
            }
        }


        Statement select(mSession);
        select << "SELECT * FROM HighScores" + whereClause.str();
        select.execute();
        RecordSet rs(select);


        HTMLRenderer renderer("High Scores", rs);
        std::stringstream ss;
        renderer.render(ss);

        std::string xml = ss.str();
        outResponse.setContentLength(xml.size());
        outResponse.send() << xml;
    }
    
    
    RequestHandler * GetHighScoreXML::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetHighScoreXML;
    }
    
    
    GetHighScoreXML::GetHighScoreXML() :
        RequestHandler(GetRequestMethod(), GetLocation(), GetContentType())
    {
    }


    void GetHighScoreXML::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {   
        Args args;
        GetArgs(inRequest.getURI(), args);
        std::stringstream whereClause;
        if (!args.empty())
        {
            whereClause << " WHERE ";
            Args::const_iterator it = args.begin(), end = args.end();
            int count = 0;
            for (; it != end; ++it)
            {
                if (count > 0)
                {
                    whereClause << " AND ";
                }
                whereClause << it->first << "='" << it->second << "'";
                count++;
            }
        }


        Statement select(mSession);
        select << "SELECT * FROM HighScores" + whereClause.str();
        select.execute();
        RecordSet rs(select);

        XMLRenderer renderer("highscores", "hs", rs);
        std::stringstream ss;
        renderer.render(ss);

        std::string xml = ss.str();
        outResponse.setContentLength(xml.size());
        outResponse.send() << xml;
    }
    
    
    RequestHandler * GetHallOfFameXML::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetHallOfFameXML;
    }
    
    
    GetHallOfFameXML::GetHallOfFameXML() :
        RequestHandler(GetRequestMethod(), GetLocation(), GetContentType())
    {
    }


    void GetHallOfFameXML::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {           
        Args args;
        GetArgs(inRequest.getURI(), args);

        Statement select(mSession);
        select << "SELECT Name, Score FROM HighScores ORDER BY Score DESC LIMIT 10";
        select.execute();
        RecordSet rs(select);

        XMLRenderer renderer("highscores", "hs", rs);
        std::stringstream ss;
        renderer.render(ss);

        std::string xml = ss.str();
        outResponse.setContentLength(xml.size());
        outResponse.send() << xml;

    }
    
    
    RequestHandler * GetHallOfFameHTML::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetHallOfFameHTML;
    }
    
    
    GetHallOfFameHTML::GetHallOfFameHTML() :
        RequestHandler(GetRequestMethod(), GetLocation(), GetContentType())
    {
    }


    void GetHallOfFameHTML::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {           
        Args args;
        GetArgs(inRequest.getURI(), args);

        Statement select(mSession);
        select << "SELECT Name, Score FROM HighScores ORDER BY Score DESC LIMIT 10";
        select.execute();
        RecordSet rs(select);

        HTMLRenderer renderer("Hall of Fame", rs);
        std::stringstream ss;
        renderer.render(ss);

        std::string html = ss.str();
        outResponse.setContentLength(html.size());
        outResponse.send() << html;
    }


    RequestHandler * GetAddHighScore::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetAddHighScore;
    }


    GetAddHighScore::GetAddHighScore() :
        RequestHandler(GetRequestMethod(), GetLocation(), GetContentType())
    {
    }


    void GetAddHighScore::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {        
        std::string body;
        ReadEntireFile("html/add.html", body);
        outResponse.setContentLength(body.size());
        outResponse.send() << body;
    }

    
    RequestHandler * PostHighScore::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new PostHighScore;
    }


    PostHighScore::PostHighScore() :
        RequestHandler(GetRequestMethod(), GetLocation(), GetContentType())
    {
    }


    void PostHighScore::generateResponse(Poco::Net::HTTPServerRequest& inRequest,
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
        std::string body = "hs/commit-succeeded?name=" + name + "&score=" + score;
        outResponse.setContentLength(body.size());
        outResponse.send() << body;
    }


    RequestHandler * CommitSucceeded::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        Args args;
        GetArgs(inRequest.getURI(), args);
        return new CommitSucceeded(GetArg(args, "name"), GetArg(args, "score"));
    }


    CommitSucceeded::CommitSucceeded(const std::string & inName, const std::string & inScore) :
        RequestHandler(GetRequestMethod(), GetLocation(), GetContentType()),
        mName(inName),
        mScore(inScore)
    {
    }


    void CommitSucceeded::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {
        std::string html = getSimpleHTML("High Score Added", MakeHTML("p", "Succesfully added highscore for " + mName + " of " + mScore + ".", HTMLFormatting_OneLiner));
        outResponse.setContentLength(html.size());
        outResponse.send() << html;
    }

} // namespace HSServer
