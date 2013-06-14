#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <stdint.h>


void encode(std::ostream& os, char c)
{
    switch (c)
    {
        case '!' : os.write("%21", 3u); return;
        case '#' : os.write("%23", 3u); return;
        case '$' : os.write("%24", 3u); return;
        case '&' : os.write("%26", 3u); return;
        case '\'': os.write("%27", 3u); return;
        case '(' : os.write("%28", 3u); return;
        case ')' : os.write("%29", 3u); return;
        case '*' : os.write("%2A", 3u); return;
        case '+' : os.write("%2B", 3u); return;
        case ',' : os.write("%2C", 3u); return;
        case '/' : os.write("%2F", 3u); return;
        case ':' : os.write("%3A", 3u); return;
        case ';' : os.write("%3B", 3u); return;
        case '=' : os.write("%3D", 3u); return;
        case '?' : os.write("%3F", 3u); return;
        case '@' : os.write("%40", 3u); return;
        case '[' : os.write("%5B", 3u); return;
        case ']' : os.write("%5D", 3u); return;
        default  :
        {
            uint8_t u;
            memcpy(&u, &c, sizeof(c));
            os.put(u);
            return;
        }
    }
}


void read()
{
    char c;
    while (std::cin.read(&c, 1))
    {
        encode(std::cout , c);
    }
}


void self_test()
{
    std::string result;
    for (int i = 0; i != 50; ++i)
    {
        std::stringstream ss;
        encode(ss, char(i));

        ss >> result;
    }
    std::cout << result << std::endl;
}


int main()
{
    //self_test();
    read();
}
