#include "HighScoreRequestHandler.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Util/Application.h"
#include <iostream>


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


    void GetAllHighScores::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                         Poco::Net::HTTPServerResponse& inResponse)
    {
        Poco::Util::Application& app = Poco::Util::Application::instance();
        app.logger().information("Request from " + inRequest.clientAddress().toString());

        inResponse.setChunkedTransferEncoding(true);
        inResponse.setContentType("text/html");


        // create a session
        Poco::Data::Session session("SQLite", "sample.db");
        Poco::Data::Statement select(session);
        select << "SELECT * FROM HighScores";
        select.execute();
        
        std::ostream& ostr = inResponse.send();
        ostr << "<html><body><p>High Score Table</p><table>";
        
        Poco::Data::RecordSet rs(select);
        for (size_t idx = 0; idx != rs.rowCount(); ++idx)
        {
            ostr << "<tr>";
            ostr << "<td>" << (int)rs.value(0, idx) << "</td>";
            ostr << "<td>" << (int)rs.value(1, idx) << "</td>";
            ostr << "</tr>";
        }
       
        ostr << "</table></body></html>";
    }

    void AddHighScore::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                                Poco::Net::HTTPServerResponse& inResponse)
    {
    }

} // namespace HSServer
