#include <iostream>
#include <string>

char gDelimiter = '\"';
static std::string gHTTP = "http";

int main()
{
    std::string word;
    while (std::cin >> word)
    {
        auto start = word.find(gHTTP);
        if (start == std::string::npos)
        {
            continue;
        }

        if (start == word.size())
        {
            continue;
        }

        //char next = word[start + gHTTP.size()];
        //if (next != 's' && next != '/')
        //{
            //continue;
        //}
        
        auto end = word.find(gDelimiter, start + gHTTP.size());
        if (end == std::string::npos)
        {
            continue;
        }
        
        std::cout << word.substr(start, end - start) << std::endl;
    }
}
