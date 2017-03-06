#include <fstream>
#include <iostream>
#include <string>
#include <vector>


//raw_timeval
// "current_time"
std::vector<int64_t> get_values(const std::string& file)
{
    std::vector<int64_t> result;

    std::ifstream ifs(file);
    int64_t value = 0;
    while (ifs >> value)
    {
        result.push_back(value);
    }

    return result;
}


int main()
{
    std::cout << get_values("raw_timeval").size() << std::endl;
    std::cout << get_values("current_time").size() << std::endl;
}
