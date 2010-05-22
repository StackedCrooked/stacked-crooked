//#include "HighScoreServer.h"
//
//using namespace HSServer;
//
//
//int main(int argc, char** argv)
//{
//    HighScoreServer app;
//	return app.run(argc, argv);
//}

#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/SQLite/Connector.h"
#include <iostream>


using namespace Poco::Data;


int main(int argc, char** argv)
{
    // register SQLite connector
    Poco::Data::SQLite::Connector::registerConnector();

    // create a session
    Session session("SQLite", "sample.db");

    // drop sample table, if it exists
    session << "DROP TABLE IF EXISTS Person", now;

    // (re)create table
    session << "CREATE TABLE Person (Name VARCHAR(30), Address VARCHAR, Age INTEGER(3))", now;

    // insert some rows
    session << "INSERT INTO Person VALUES('Homer Simpson', 'Springfield', 42)", now;
    session << "INSERT INTO Person VALUES('Marge Simpson', 'Springfield', 38)", now;
    session << "INSERT INTO Person VALUES('Bart Simpson', 'Springfield', 12)", now;
    session << "INSERT INTO Person VALUES('Lisa Simpson', 'Springfield', 10)", now;

    // get the data
    Statement select(session);
    select << "SELECT * FROM Person";
    select.execute();

    // create a recordset and print the column names and data
    RecordSet rs(select);

    std::cout << "Number of rows: " << rs.rowCount() << std::endl;

    std::cout << "Print ENTER to quit.";
    std::cin.get();
    return 0;
}