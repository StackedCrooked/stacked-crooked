#include "Pool.h"
#include <string>


std::size_t socket_write(const void* src, std::size_t n);
std::size_t socket_read(char* src, std::size_t n);



Pool<std::string> pool;


void send(std::string&& s)
{
    auto written = socket_write(s.data(), s.size());
    while (written < s.size())
    {
        written += socket_write(s.data() + written, s.size() - written);
    }
    pool.recycle(std::move(s));
}


std::string receive()
{
    std::string s = pool.get();

    auto data = s.data();
    auto size = s.size();
    auto n = socket_read(data, size);
    while (n)
    {
        data += n;
        size -= n;
        if (size == 0)
        {
            s.resize(
        }
        n = socket_read(data, size);
    }
    num_read += socket_read(s.data() + num_read, s.size() - num_read);
    while (num_read > 0)
    {

        socket_receive(s.data() + old_size, num_read);
    }
    return s;
}






int main()
{

    auto s = pool.get();
    send(s)


    pool.recycle(std::move(s));
}
