#include "HighScoreRequestHandler.h"
#include "HTMLElements.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Util/Application.h"
#include <iostream>


using namespace Poco::Data;


namespace HSServer
{
    
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


    void CreateTable()
    {
        SessionFactory & sessionFactory(SessionFactory::instance());
        Session session(sessionFactory.create("SQLite", "HighScores.db"));
        session << "CREATE TABLE IF NOT EXISTS HighScores(Score INTEGER(5))", now;
        
    }


    void GenerateResponse(std::ostream & ostr)
    {
        HTMLElements::html html(ostr);
        HTMLElements::body body(ostr);
        HTMLElements::p(ostr, "High Score Table");
        HTMLElements::table table(ostr);
        
        Session session("SQLite", "HighScores.db");
        Statement select(session);
        select << "SELECT * FROM HighScores";
        select.execute();
        RecordSet rs(select);
        for (size_t idx = 0; idx != rs.rowCount(); ++idx)
        {
            HTMLElements::tr tr(ostr);
            HTMLElements::td td(ostr, rs.value(0, idx));
        }
        ostr << "</table></body></html>";
    }


    void GetAllHighScores::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                         Poco::Net::HTTPServerResponse& inResponse)
    {
        Poco::Util::Application& app = Poco::Util::Application::instance();
        app.logger().information("Request from " + inRequest.clientAddress().toString());

        CreateTable();

        inResponse.setChunkedTransferEncoding(true);
        inResponse.setContentType("text/html");
        GenerateResponse(inResponse.send());
    }

    void AddHighScore::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                                Poco::Net::HTTPServerResponse& inResponse)
    {
    }

} // namespace HSServer
