#include <fstream>

int main()
{
	std::ofstream f("output.txt");
	f << "Hello World!" << std::endl;
}
