#include <iostream>
#include <string>
#include <vector>


std::vector<std::string> split(const std::string & text, const std::string & delim)
{
    std::vector<std::string> result;
    std::string::size_type begin = 0;
    while (true)
    {
        std::string::size_type end = text.find(delim, begin);
        if (end == std::string::npos)
        {
            result.push_back(text.substr(begin, std::string::npos));
            break;
        }

        result.push_back(text.substr(begin, end - begin));
        begin = end + delim.size();
    }
    return result;
}


std::ostream & operator<<(std::ostream & os, const std::vector<std::string> & items)
{
    for (auto item: items)
    {
        os << "\"" << item << "\" ";
    }
    return os << std::endl;
}


int main()
{
    std::string test = "a";
    std::cout << split(test, ".") << std::endl;
    std::cout << "Done." << std::endl;
}
