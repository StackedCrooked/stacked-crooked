#include "Config.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/Session.h"
#include "Poco/Logger.h"


namespace HSServer {


using Poco::Data::Statement;
using Poco::Data::use;


// Declaration only.
Poco::Logger & GetLogger();


HTMLErrorResponse::HTMLErrorResponse(const std::string & inErrorMessage) :
    RequestHandler(ErrorPage::Id, Method_Get, ContentType_TextHTML),
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

    Statement insert(getSession());
    insert << "INSERT INTO HighScores VALUES(NULL, strftime('%s', 'now'), ?, ?)", use(name), use(score);
    insert.execute();

    // Return an URL instead of a HTML page.
    // This is because the client is the JavaScript application in this case.
    std::string body = ResourceManager::Instance().getResourceLocation(GetResourceId());
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

    Statement insert(getSession());
    insert << sql;
    insert.execute();

    // Return a message indicating success.
    std::string body = "Succesfully performed the following SQL statement: " + sql;
    outResponse.setContentLength(body.size());
    outResponse.send() << body;
}


void RegisterRequestHandlers(RequestHandlerFactory & ioRequestHandlerFactory)
{
    // HighScore
    ioRequestHandlerFactory.registerRequestHandler<GetHighScore_HTML>();
    ioRequestHandlerFactory.registerRequestHandler<GetHighScore_XML>();
    ioRequestHandlerFactory.registerRequestHandler<GetHighScore_Text>();
    ioRequestHandlerFactory.registerRequestHandler<PostHightScore>();
    ioRequestHandlerFactory.registerRequestHandler<DeleteHighScore>();

    // HighScorePostForm
    ioRequestHandlerFactory.registerRequestHandler<GetHighScorePostForm>();

    // HighScoreDeleteForm
    ioRequestHandlerFactory.registerRequestHandler<GetHighScoreDeleteForm>();

    // HallOfFame
    ioRequestHandlerFactory.registerRequestHandler<GetHallOfFame_HTML>();
    ioRequestHandlerFactory.registerRequestHandler<GetHallOfFame_XML>();
    ioRequestHandlerFactory.registerRequestHandler<GetHallOfFame_Text>();
}


} // namespace HSServer
