#include "Gst/Support.h"
#include "Gst/VideoPlayer.h"
#include "Gst/MainLoop.h"
#include <iostream>
#include <sstream>


void run(const std::string & inPath)
{
	Gst::MainLoop mainLoop;
	gst_init(0, 0);
	Gst::VideoPlayer vp(0, inPath);
	mainLoop.run();
}

int main(int argc, const char ** argv)
{
	try
	{
		if (argc < 2)
		{
            std::stringstream ss;
            ss << "Usage: " << argv[0] << " FILE" << std::endl;
            throw std::runtime_error(ss.str());
        }
        run(argv[1]);
	}
	catch (const std::exception & exc)
	{
		std::cout << "Exception caught in main: " << exc.what() << std::endl;
	}
	return 0;
}
