#include <iostream>
#include <boost/filesystem.hpp>


std::string basename(const std::string& dir)
{
    return dir.substr(dir.rfind('/') + 1);
}


void find(const std::string& dir, const std::string& name)
{
    using namespace boost::filesystem;
	directory_iterator it(dir), end;
	for (; it != end; ++it)
	{
		auto entry = *it;
		auto path = entry.path().string();
        if (is_directory(*it))
        {
            find(path, name);
        }
		else if (basename(path) == name)
		{
			std::cout << path << std::endl;
		}
	}
}

int main(int argc, char** argv)
{
    if (argc != 3) throw argc;
    std::string dir = argv[1];
    std::string match = argv[2];
	find(dir, match);
}
