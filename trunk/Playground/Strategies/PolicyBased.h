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
    std::string serialize(const std::string & str)
    {
        return str;
    }

    template<typename T>
    std::string serialize(const T & t)
    {
        std::stringstream ss;
        ss << t;
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
        std::map<int, std::vector<std::map<int, std::string>>> object;

        object[0].push_back({
            { 0, "zero" },
            { 1, "one"  },
            { 2, "two" }
        });
        object[1].push_back({
            { 3, "three" },
            { 4, "four"  },
            { 5, "five" }
        });
        arch.store(object);
    }
};


Test t;


#endif // POLICYBASED_H
