#include "RequestHandler.h"
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

    
    RequestHandler::RequestHandler(RequestMethod inRequestMethod, const std::string & inLocation, const std::string & inResponseContentType) :
        mSession(SessionFactory::instance().create("SQLite", "HighScores.db")),
        mRequestMethod(inRequestMethod),
        mLocation(inLocation),
        mResponseContentType(inResponseContentType)
    {
        // Create the table if it doesn't already exist
        static bool fFirstTime(true);
        if (fFirstTime)
        {
            mSession << "CREATE TABLE IF NOT EXISTS HighScores(Id INTEGER PRIMARY KEY, Name VARCHAR(20), Score INTEGER(5))", now;
            fFirstTime = false;
        }
    }


    std::string RequestHandler::getSimpleHTML(const std::string & inTitle, const std::string & inBody)
    {
        std::string doc;
        HSServer::ReadEntireFile("html/html-document-template.html", doc);
        return Poco::replace<std::string>(Poco::replace<std::string>(doc, "{{title}}", inTitle), "{{body}}", inBody);
    }


    const std::string & RequestHandler::getResponseContentType() const
    {
        return mResponseContentType;
    }


    const std::string & RequestHandler::getPath() const
    {
        return mLocation;
    }

    
    void RequestHandler::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                       Poco::Net::HTTPServerResponse& outResponse)
    {
        
        GetLogger().information("Request from " + inRequest.clientAddress().toString());

        outResponse.setChunkedTransferEncoding(true);
        outResponse.setContentType(getResponseContentType());

        try
        {
            generateResponse(inRequest, outResponse);
        }
        catch (const std::exception & inException)
        {
            GetLogger().error(inException.what());
        }
    }

    
    HTMLResponder::HTMLResponder(RequestMethod inRequestMethod,
                                 const std::string & inLocation) :
        RequestHandler(inRequestMethod, inLocation, "text/html")
    {
    }

    
    XMLResponder::XMLResponder(RequestMethod inRequestMethod, const std::string & inLocation) :
        RequestHandler(inRequestMethod, inLocation, "text/xml")
    {
    }

    
    PlainTextResponder::PlainTextResponder(RequestMethod inRequestMethod, const std::string & inLocation) :
        RequestHandler(inRequestMethod, inLocation, "text/plain")
    {
    }


    HTMLErrorResponse::HTMLErrorResponse(const std::string & inErrorMessage) :
        HTMLResponder(RequestMethod_Get, ""),
        mErrorMessage(inErrorMessage)
    {
    }


    void HTMLErrorResponse::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {
        std::string html = getSimpleHTML("Error", MakeHTML("p", mErrorMessage));
        outResponse.setContentLength(html.size());
        outResponse.send() << html;
    }
    
    
    RequestHandler * GetHighScoreHTML::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetHighScoreHTML;
    }
    
    
    GetHighScoreHTML::GetHighScoreHTML() :
        HTMLResponder(GetRequestMethod(), GetLocation())
    {
    }


    void GetHighScoreHTML::getRows(const Poco::Data::RecordSet & inRecordSet, std::string & outRows)
    {
        for (size_t rowIdx = 0; rowIdx != inRecordSet.rowCount(); ++rowIdx)
        {   
            outRows += "<tr>";
            for (size_t colIdx = 0; colIdx != inRecordSet.columnCount(); ++colIdx)
            {                
                outRows += "<td>";
                std::string rawCellValue;
                inRecordSet.value(colIdx, rowIdx).convert(rawCellValue);
                outRows += URIEncode(rawCellValue);
                outRows += "</td>";
            }
            outRows += "</tr>\n";
        }
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
        
        std::string rows;
        getRows(rs, rows);
        std::string html = getSimpleHTML("High Scores", MakeHTML("table", rows));
        outResponse.setContentLength(html.size());
        outResponse.send() << html;
    }
    
    
    RequestHandler * GetHighScoreXML::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetHighScoreXML;
    }
    
    
    GetHighScoreXML::GetHighScoreXML() :
        XMLResponder(GetRequestMethod(), GetLocation())
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
        select << "SELECT Name, Score FROM HighScores" + whereClause.str();
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
        XMLResponder(GetRequestMethod(), GetLocation())
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


    RequestHandler * GetAddHighScore::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetAddHighScore;
    }


    GetAddHighScore::GetAddHighScore() :
        HTMLResponder(GetRequestMethod(), GetLocation())
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
        PlainTextResponder(GetRequestMethod(), GetLocation())
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
        insert << "INSERT INTO HighScores VALUES(NULL, ?, ?)", use(name), use(score);
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
        HTMLResponder(GetRequestMethod(), GetLocation()),
        mName(inName),
        mScore(inScore)
    {
    }


    void CommitSucceeded::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
    {
        std::string html = getSimpleHTML("High Score Added", MakeHTML("p", "Succesfully added highscore for " + mName + " of " + mScore + "."));
        outResponse.setContentLength(html.size());
        outResponse.send() << html;
    }

} // namespace HSServer
