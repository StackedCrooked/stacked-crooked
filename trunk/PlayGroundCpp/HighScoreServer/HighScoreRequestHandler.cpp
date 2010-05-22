#include "HighScoreRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/Application.h"


namespace HSServer
{

    void HighScoreRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                                Poco::Net::HTTPServerResponse& inResponse)
    {
        Poco::Util::Application& app = Poco::Util::Application::instance();
        app.logger().information("Request from " + inRequest.clientAddress().toString());

        inResponse.setChunkedTransferEncoding(true);
        inResponse.setContentType("text/html");

        std::ostream& ostr = inResponse.send();
        ostr << "<html><head><body>You have nice high scores!</body></html>";
    }

} // namespace HSServer
