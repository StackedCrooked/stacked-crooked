#include "HighScoreRequestHandler.h"
#include "HTMLElements.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Util/Application.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>


using namespace Poco::Data;
using namespace HTML;


namespace HSServer
{
    
    typedef std::map<std::string, std::string> Args;

    void GetArgs(const std::string & inURI, Args & outArgs)
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

    
    HighScoreRequestHandler::HighScoreRequestHandler() :
        mSession(SessionFactory::instance().create("SQLite", "HighScores.db"))
    {
        // Create the table if it doesn't already exist
        mSession << "CREATE TABLE IF NOT EXISTS HighScores(Id INTEGER PRIMARY KEY, Name VARCHAR(20), Score INTEGER(5))", now;
    }

    
    void HighScoreRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                                Poco::Net::HTTPServerResponse& inResponse)
    {
        Poco::Util::Application& app = Poco::Util::Application::instance();
        app.logger().information("Request from " + inRequest.clientAddress().toString());

        inResponse.setChunkedTransferEncoding(true);
        inResponse.setContentType("text/html");

        std::ostream & outStream = inResponse.send();
        HTML::CurrentOutStream currentOutputStream(outStream);
        try
        {
            generateResponse(mSession, outStream);
        }
        catch (const std::exception & inException)
        {
            app.logger().critical(inException.what());
        }
    }    
    
    
    DefaultRequestHandler * DefaultRequestHandler::Create(const std::string & inURI)
    {
        return new DefaultRequestHandler;
    }


    void DefaultRequestHandler::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        HTML_html html;
        HTML_body body;
        HTML_h1 h1("High Score Server");
        HTML_p p0("There is nothing here.");
        HTML_p p1("Really.");
        HTML_b b("I'm bold.");
        HTML_br br0;
        HTML_i i("I'm scheef.");
        HTML_br br1;
        HTML_u u("I'm underlined.");


        {
            HTML_p p2;
            Write("This is a big scoped paragraph.");
        }

        {
            HTML_p p1;
            HTML_b b1;
            HTML_i i1;
            HTML_u u1;
            Write("Nested");
            HTML_br br0;
            Write("and various");
            HTML_br br1;
            Write("attributes");
            HTML_br br2;
        }
        {
            HTML_table table;

            // Table Header
            {
                HTML_thead thead0;
                HTML_tr tr0;
                HTML_th th0("Name");
                HTML_th th1("Score");
            }

            // Row 0
            {
                HTML_tr row;
                {
                    HTML_td td0;
                    Write("FRA");
                }
                HTML_td td1("100");
            }

            // Row 1
            {
                HTML_tr row;
                HTML_td td0("JON");
                {
                    HTML_td td1;
                    Write("200");
                    HTML_br br0;
                    Write("/");
                    HTML_br br1;
                    Write("250");
                }
            }

            // Row 2
            {
                HTML_tr row;
                HTML_td td0("PHI");
                HTML_td td1("250");
            }
        }
    }
    
    
    GetAllHighScores * GetAllHighScores::Create(const std::string & inURI)
    {
        return new GetAllHighScores;
    }


    void GetAllHighScores::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        HTML_html html;
        HTML_body body;
        HTML_p p("High Score HTML_table");
        HTML_table table;
        Statement select(inSession);
        select << "SELECT * FROM HighScores";
        select.execute();
        RecordSet rs(select);
        for (size_t rowIdx = 0; rowIdx != rs.rowCount(); ++rowIdx)
        {   
            HTML_tr tr;
            for (size_t colIdx = 0; colIdx != rs.columnCount(); ++colIdx)
            {
                HTML_td td;
                Poco::DynamicAny v = rs.value(colIdx, rowIdx);
                if (v.isString())
                {
                    std::string s = v;
                    Write(s);
                }
                else if (v.isNumeric())
                {
                    int i = v;
                    std::string s = boost::lexical_cast<std::string>(i);
                    Write(s);
                }                
            }
        }
    }
    
    
    AddHighScore * AddHighScore::Create(const std::string & inURI)
    {
        Args args;
        GetArgs(inURI, args);

        Args::iterator it = args.find("name");
        if (it == args.end())
        {
            throw std::runtime_error("Could not find 'name' argument in GET URL for AddHighScore method.");
        }

        std::string name = it->second;

        it = args.find("score");
        if (it == args.end())
        {
            // TODO: create class MissingArgument
            throw std::runtime_error("Could not find 'score' argument in GET URL for AddHighScore method.");
        }

        int score = 0;
        try
        {
            score = boost::lexical_cast<int>(it->second);
        }
        catch (const boost::bad_lexical_cast & )
        {
            throw std::runtime_error("Argument 'score' must be of type INTEGER.");
        }

        return new AddHighScore(HighScore(name, score));
    }


    AddHighScore::AddHighScore(const HighScore & inHighScore) :
        mHighScore(inHighScore)
    {
    }


    void AddHighScore::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        HTML_html html;
        HTML_body body;

        Statement insert(inSession);

        insert << "INSERT INTO HighScores VALUES(NULL, ?, ?)", use(mHighScore.name()),
                                                               use(mHighScore.score());
        insert.execute();

        std::stringstream ss;
        ss << "Added High Score: " << mHighScore.name() << ": " << mHighScore.score();
        HTML_p(ss.str());
    }

} // namespace HSServer
