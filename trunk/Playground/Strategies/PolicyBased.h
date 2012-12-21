#ifndef POLICYBASED_H
#define POLICYBASED_H


#include <map>
#include <sstream>
#include <string>
#include <vector>


template<class Serializer,
         class Compressor,
         class Output>
class Archiver : private Serializer,
                 private Compressor,
                 private Output
{
public:
    template<typename T>
    void store(const T & value)
    {
        this->output(this->compress(this->serialize(value)));
    }
};


struct JSON
{
    std::string serialize(int n)
    {
        std::stringstream ss;
        ss << n;
        return ss.str();
    }

    std::string serialize(const std::string & t)
    {
        std::stringstream ss;
        ss << "\"" << t << "\"";
        return ss.str();
    }

    template<typename T>
    std::string serialize(const std::vector<T> & vec)
    {
        std::stringstream ss;
        ss << "[";
        for (typename std::vector<T>::size_type idx = 0; idx != vec.size(); ++idx)
        {
            if (idx != 0)
            {
                ss << ", ";
            }
            ss << serialize(vec[idx]);
        }
        ss << "]";
        return ss.str();
    }

    template<typename T, typename U>
    std::string serialize(const std::map<T, U> & map)
    {
        std::stringstream ss;
        ss << "{ ";
        bool first = true;
        for (const auto & entry : map)
        {
            if (!first)
            {
                ss << ", ";
            }
            ss << serialize(entry.first) << ": " << serialize(entry.second);
            first = false;
        }
        ss << " }";
        return ss.str();
    }
};


struct NoCompression
{
    std::string compress(const std::string & str)
    {
        return str;
    }
};


struct StdOut
{
    void output(const std::string & str)
    {
        std::cout << str << std::endl;
    }
};


struct Test
{
    Test()
    {
        Archiver<JSON, NoCompression, StdOut> arch;
        typedef std::map<std::string, std::vector<int>> Data;
        Data data = {
            { "ip_1" , { 10, 0, 0, 1 } },
            { "ip_2" , { 10, 0, 0, 2 } }
        };
        arch.store(data);
    }
};


Test t;


#endif // POLICYBASED_H
