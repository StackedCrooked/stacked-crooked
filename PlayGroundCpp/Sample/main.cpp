#include "Poco/File.h"
#include "Poco/Thread.h"
#include <boost/shared_ptr.hpp>
#include <iostream>


//
// README
// 
// Introduction
// ------------
// Try building this project. It should work out of the box without requiring
// any customization. If it doesn't then send me an email.
//
//
// Provided libraries
// ------------------
//   * Boost v1.42
//   * Poco v1.3.6p2
//
//
// Poco notes
// ----------
// A few modifications have been made to the project files to bring them in
// alignment with the entire solution:
//   * only static builds are supported (all projects produce a .lib file)
//   * build configurations are reduced to "Debug" and "Release"
//   * output directory is set to "Build" (relative to the solution directory)
//
// You can delete unneeded Poco projects from the Solution Explorer.
//
//
// Boost notes
// -----------
// The provided boost repository is a subset of the complete boost repository.
// The reason for this is that I find that the boost library takes too much
// time to checkout from Subversion while I only use a fraction of it.
//
// The current subset was created with the boost bcp utility and contains the
// following libraries:
//   * bind
//   * function
//   * lexical_cast
//   * noncopyable
//   * optional
//   * program_options
//   * smart_ptr
//   * signals2
//   * + any depending libraries (auto-included by the bcp utility)


int main()
{
	Poco::File f;
	Poco::Thread t;
    boost::shared_ptr<int> i(new int(1));
    std::cout << "This is a sample application." << std::endl;
    std::cout << "Print ENTER to continue." << std::endl;
    std::cin.get();
    return 0;
}
