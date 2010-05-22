#include "HighScoreServer.h"
#include "SQLiteSupport.h"

using namespace HSServer;


int main(int argc, char** argv)
{
    SQLiteSupport sqlite;
    HighScoreServer app;
	return app.run(argc, argv);
}
