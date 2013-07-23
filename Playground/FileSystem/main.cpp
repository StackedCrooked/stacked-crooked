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
            std::cout << __LINE__ << std::endl;
            this->import(dir);
        }))
    {   
        std::cout << __LINE__ << std::endl;
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
        std::cout << "depth: " << depth << std::endl;
        using namespace boost::filesystem;
        for (auto it = directory_iterator(dir), end = directory_iterator(); it != end; ++it)
        {
            auto entry = *it;
            auto str = entry.path().string();
            std::cout << "Got entry: " << str << std::endl;
            if (is_regular_file(entry))
            {
                std::cout << str << " is a file" << std::endl;
                import_file(entry.path().string());
            }
            else if (is_directory(entry))
            {
                std::cout << str << " is a directory" << std::endl;
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
    std::cout << __LINE__ << std::endl;
    Importer importer(".");
    for (;;)
    {
        std::cout << "popping the future" << std::endl;
        std::future<std::string> fut = importer.pop();
        std::cout << "wait_for 10 ms" << std::endl;
        auto status = fut.wait_for(std::chrono::milliseconds(10));
        std::cout << "finished waiting, getting result" << std::endl;
        if (status == std::future_status::ready)
        {
            std::cout << "POPPED: " << fut.get() << std::endl;
        }
        else if (status == std::future_status::timeout)
        {
            throw std::runtime_error("Timeout");
        }
        else
        {
            throw std::runtime_error("Not started yet!");
        }
    }
    std::cout << __LINE__ << std::endl;
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

