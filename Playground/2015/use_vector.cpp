#include <vector>

void test(std::vector<int>& vec, int i)
{
    vec.push_back(i);
    vec.clear();
    vec.size();
    vec.push_back(vec.end() - vec.begin());
}
