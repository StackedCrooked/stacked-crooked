#include "HighScoreRequestHandler.h"
#include "FileUtils.h"
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
    
    HighScoreRequestHandler::HighScoreRequestHandler() :
        mSession(SessionFactory::instance().create("SQLite", "HighScores.db"))
    {
        // Create the table if it doesn't already exist
        mSession << "CREATE TABLE IF NOT EXISTS HighScores(Id INTEGER PRIMARY KEY, Name VARCHAR(20), Score INTEGER(5))", now;
    }

    void HighScoreRequestHandler::GetArgs(const std::string & inURI, Args & outArgs)
    {
        if (inURI.empty())
        {
            return;
        }

        if (inURI[0] != '/')
        {
            return;
        }

        std::string argString = inURI.substr(1, inURI.size() - 1);
        Poco::StringTokenizer tokenizer(argString, "&", Poco::StringTokenizer::TOK_IGNORE_EMPTY |
                                                        Poco::StringTokenizer::TOK_TRIM);

        Poco::StringTokenizer::Iterator it = tokenizer.begin(), end = tokenizer.end();
        for (; it != end; ++it)
        {
            const std::string & pair = *it;
            Poco::StringTokenizer t(pair, "=", Poco::StringTokenizer::TOK_IGNORE_EMPTY |
                                               Poco::StringTokenizer::TOK_TRIM);
            if (t.count() != 2)
            {
                continue;
            }
            outArgs.insert(std::make_pair(t[0], t[1]));
        }
    }


    std::string HighScoreRequestHandler::getContentType() const
    {
        return "text/html";
    }

    
    void HighScoreRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                                Poco::Net::HTTPServerResponse& inResponse)
    {
        Poco::Util::Application& app = Poco::Util::Application::instance();
        app.logger().information("Request from " + inRequest.clientAddress().toString());

        inResponse.setChunkedTransferEncoding(true);
        inResponse.setContentType(getContentType());

        std::ostream & outStream = inResponse.send();
        try
        {
            generateResponse(mSession, outStream);
        }
        catch (const std::exception & inException)
        {
            app.logger().critical(inException.what());
        }
    }    
    
    
    HighScoreRequestHandler * DefaultRequestHandler::Create(const std::string & inURI)
    {
        return new DefaultRequestHandler;
    }


    void DefaultRequestHandler::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        std::ifstream html("html/index.html");
        Poco::StreamCopier::copyStream(html, ostr);
    }


    ErrorRequestHandler::ErrorRequestHandler(const std::string & inErrorMessage) :
        mErrorMessage(inErrorMessage)
    {
    }


    void ErrorRequestHandler::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        ostr << "<html><body>";
        ostr << "<h1>Error</h1>";
        ostr << "<p>" + mErrorMessage + "</p>";
        ostr << "</body></html>";
    }
    
    
    HighScoreRequestHandler * GetAllHighScores::Create(const std::string & inURI)
    {
        return new GetAllHighScores;
    }


    std::string GetStringValue(const Poco::DynamicAny & inDynamicValue, const std::string & inDefault)
    {
        if (inDynamicValue.isString())
        {
            return static_cast<std::string>(inDynamicValue);
        }
        else if (inDynamicValue.isNumeric())
        {
            return boost::lexical_cast<std::string>(static_cast<int>(inDynamicValue));
        }
        return inDefault;
    }


    void GetAllHighScores::getRows(const Poco::Data::RecordSet & inRecordSet, std::string & outRows)
    {
        for (size_t rowIdx = 0; rowIdx != inRecordSet.rowCount(); ++rowIdx)
        {   
            outRows += "<tr>";
            for (size_t colIdx = 0; colIdx != inRecordSet.columnCount(); ++colIdx)
            {                
                outRows += "<td>";
                outRows += GetStringValue(inRecordSet.value(colIdx, rowIdx), "(Unknown DynamicAny)");
                outRows += "</td>";
            }
            outRows += "</tr>\n";
        }
    }


    void GetAllHighScores::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {   
        Statement select(inSession);
        select << "SELECT * FROM HighScores";
        select.execute();
        RecordSet rs(select);

        std::string html;
        HSServer::ReadEntireFile("html/getall.html", html);
        std::string rows;
        getRows(rs, rows);
        ostr << Poco::replace<std::string>(html, "{{ROWS}}", rows);
    }
    
    
    HighScoreRequestHandler * AddHighScore::Create(const std::string & inURI)
    {
        return new AddHighScore;
    }


    AddHighScore::AddHighScore()
    {
    }


    void AddHighScore::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        std::ifstream htmlFile("html/add.html");
        Poco::StreamCopier::copyStream(htmlFile, ostr);
    }
    
    
    HighScoreRequestHandler * CommitHighScore::Create(const std::string & inURI)
    {
        Args args;
        GetArgs(inURI, args);

        Args::iterator it = args.find("name");
        if (it == args.end())
        {
            throw std::runtime_error("Could not find 'name' argument in the URL for CommitHighScore method.");
        }

        std::string name = it->second;

        it = args.find("score");
        if (it == args.end())
        {
            // TODO: create class MissingArgument
            throw std::runtime_error("Could not find 'score' argument in GET URL for CommitHighScore method.");
        }

        int score = 0;
        try
        {
            score = boost::lexical_cast<int>(it->second);
        }
        catch (const boost::bad_lexical_cast & )
        {
            return new ErrorRequestHandler("Score should be a positive number.");
        }

        return new CommitHighScore(HighScore(name, score));
    }


    CommitHighScore::CommitHighScore(const HighScore & inHighScore) :
        mHighScore(inHighScore)
    {
    }


    void CommitHighScore::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {        
        Statement insert(inSession);
        insert << "INSERT INTO HighScores VALUES(NULL, ?, ?)", use(mHighScore.name()),
                                                               use(mHighScore.score());
        insert.execute();

        // Return an URL instead of a HTML page.
        // This is because the client is the JavaScript application in this case.
        ostr << "http://localhost/hs/commit-succeeded&name=" << mHighScore.name() << "&score=" << mHighScore.score();
    }


    std::string CommitHighScore::getContentType() const
    {
        return "text/plain";
    }
    
    
    HighScoreRequestHandler * CommitSucceeded::Create(const std::string & inURI)
    {
        Args args;
        GetArgs(inURI, args);

        Args::iterator it = args.find("name");
        if (it == args.end())
        {
            throw std::runtime_error("Could not find 'name' argument in the URL for CommitHighScore method.");
        }

        std::string name = it->second;

        it = args.find("score");
        if (it == args.end())
        {
            // TODO: create class MissingArgument
            throw std::runtime_error("Could not find 'score' argument in GET URL for CommitHighScore method.");
        }

        int score = 0;
        try
        {
            score = boost::lexical_cast<int>(it->second);
        }
        catch (const boost::bad_lexical_cast & )
        {
            throw std::runtime_error("Score is not of type INTEGER");
        }

        return new CommitSucceeded(HighScore(name, score));
    }


    CommitSucceeded::CommitSucceeded(const HighScore & inHighScore) :
        mHighScore(inHighScore)
    {
    }


    void CommitSucceeded::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        ostr << "<html>";
        ostr << "<body>";
        ostr << "<p>";
        ostr << "Succesfully added highscore for " << mHighScore.name() << " of " << mHighScore.score() << ".";
        ostr << "</p>";
        ostr << "</body>";
        ostr << "</html>";
    }

} // namespace HSServer
