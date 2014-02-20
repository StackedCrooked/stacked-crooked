#include <assert.h>
#include <iostream>
#include <string>
#include <vector>


template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    for (auto& el : vec)
    {
        os << el << ' ';
    }
    return os;
}


struct zeroes
{
    zeroes(const char* bb, const char* b, const char* e) : bb(bb), b(b), e(e) {}

    const char* bb;
    const char* b;
    const char* e;

    friend std::ostream& operator<<(std::ostream& os, zeroes z)
    {
        return os << "[" << (z.b - z.bb) << ", " << (z.e - z.bb) << ") ";
    }

    std::size_t size() const { return e - b; }
};



enum State
{
    Initial,
    Group,
    GroupColon,
    Zero,
    ZeroColon
};




std::vector<zeroes> find_zeroes_offset(const char* b, const char* e)
{
    std::vector<zeroes> result;
    State state = Initial;

    const char* zeroes_begin = NULL;
    const char* zeroes_end = NULL;

    for (auto i = b; i != e; ++i)
    {
        char c = *i;

        switch (state)
        {
            case Initial: switch (c)
            {
                case '0': state = Zero; zeroes_begin = zeroes_end = i; continue;
                case ':': result.clear(); return result;
                default: state = Group; continue;
            }
            case Group: switch (c)
            {
                case ':': state = GroupColon; continue;
                default: continue;
            }
            case GroupColon: switch (c)
            {
                case '0': state = Zero; zeroes_begin = i; continue;
                case ':': result.clear(); return result;
                default: state = Group; continue;
            }
            case Zero: switch (c)
            {
                case '0': continue;
                case ':': state = ZeroColon; zeroes_end = i; continue;
                default:
                {
                    result.push_back(zeroes(b, zeroes_begin, zeroes_end));
                    zeroes_begin = zeroes_end = NULL;
                    state = Group;
                    continue;
                }
            }
            case ZeroColon: switch(c)
            {
                case '0': continue;
                case ':': result.clear(); return result;
                default:
                {
                    result.push_back(zeroes(b, zeroes_begin, zeroes_end));
                    zeroes_begin = zeroes_end = NULL;
                    state = Group;
                    continue;
                }

            }
        }
    }

    if (state == Zero)
    {
        result.push_back(zeroes(b, zeroes_begin, e));
    }
    return result;
}


int main()
{
    std::string data = "fe80:0000:0000:0000:02ff:23ff:fe00:0002";
    auto vec = find_zeroes_offset(data.data(), data.data() + data.size());
    std::cout << "Number of intervals: " << vec.size() << std::endl;
    std::cout << vec << std::endl;

}
