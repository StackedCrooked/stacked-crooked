#define BOOST_FILESYSTEM_DYN_LINK 1
#include <deque>
#include <future>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "tbb/concurrent_queue.h"

using boost::filesystem::is_directory;
using boost::filesystem::is_regular_file;
using boost::filesystem::exists;

std::string get_extension(const std::string& file)
{
    return file.substr(file.rfind('.') + 1);
}


struct Importer
{
    Importer(const std::string& dir) :
        fut(std::async(std::launch::async, [=]{ 
            this->import(dir);
        }))
    {   
    }

    ~Importer()
    {
        
    }
    
    std::future<std::string> pop()
    {
        return std::async(std::launch::async, [=]() -> std::string {
            std::string file;
            queue.pop(file);
            return file;
        });
    }
    
private:
    void import_dir_impl(const std::string& dir, int depth)
    {
        using namespace boost::filesystem;
        for (auto it = directory_iterator(dir), end = directory_iterator(); it != end; ++it)
        {
            auto entry = *it;
            auto str = entry.path().string();
            if (is_regular_file(entry))
            {
                import_file(entry.path().string());
            }
            else if (is_directory(entry))
            {
                import_dir_impl(entry.path().string(), depth + 1);
            }
            else
            {
                std::cerr << "Not a file nor a dir: " << entry << std::endl;
            }
        }
    }
    
    void import_dir(const std::string& dir)
    {
        import_dir_impl(dir, 0);
    }
    void import_file(const std::string& file)
    {
        // check extension
        // if image then push to queue
        if (is_supported(file))
        {
            queue.push(file);
        }
    }
    
    bool is_supported(std::string file)
    {
        static const std::set<std::string> fExtensions = { "jpg", "png" };
        boost::to_lower(file);
        return fExtensions.count(get_extension(file));
    }
    
    void import(std::string dir)
    {
        import_dir(dir);
    }
    
    
    std::deque<std::string> imported;
    std::future<void> fut;
    tbb::concurrent_bounded_queue<std::string> queue;
};

void test()
{
    Importer importer(".");
    for (;;)
    {
        std::future<std::string> fut = importer.pop();
        auto status = fut.wait_for(std::chrono::milliseconds(10));
        if (status == std::future_status::ready)
        {
            std::cout << "Got: " << fut.get() << std::endl;
        }
        else if (status == std::future_status::timeout)
        {
            std::cerr << "TIMOUT!" << std::endl;
            std::abort();
        }
        else
        {
            std::cerr << "UNEXPECTED FUTURE_STATUS" << std::endl;
            std::abort();
        }
    }
}

int main()
{
    try
    {
        test();
    }
    catch (const std::exception& exc)
    {
        std::cerr << exc.what() << std::endl;
    }
}

