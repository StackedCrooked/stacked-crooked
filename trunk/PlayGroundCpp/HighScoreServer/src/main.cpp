#include <iostream>
#include "HighScoreServer.h"


using namespace HSServer;


int main(int argc, char** argv)
{
    HighScoreServer app;
	return app.run(argc, argv);
}
