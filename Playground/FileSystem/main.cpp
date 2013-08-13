#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
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

typedef std::function<bool(const std::string&)> Matcher;

struct Importer
{
    Importer(const std::string& dir, const Matcher& inMatcher) :
        mMatcher(inMatcher)
    {   
        std::thread([=]{ this->import(dir); }).detach();
    }

    std::future<std::string> pop()
    {
        auto prom = std::make_shared<std::promise<std::string>>();
        std::thread([=] {
            std::string file;
            queue.pop(file);
            prom->set_value(file);
        }).detach();
        return prom->get_future();
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
    
    
    Matcher mMatcher;
    std::deque<std::string> imported;
    std::future<void> fut;
    tbb::concurrent_bounded_queue<std::string> queue;
};

void test(Importer& importer)
{
    for (;;)
    {
        std::future<std::string> fut = importer.pop();
        auto status = fut.wait_for(std::chrono::milliseconds(10));
        if (status == std::future_status::ready)
        {
            std::cout << "Imported: " << fut.get() << std::endl;
        }
        else if (status == std::future_status::timeout)
        {
            throw std::runtime_error("Timeout!");
        }
        else
        {
            throw std::runtime_error("Unexpected status!");
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) throw argc;
    std::string match = argv[2];
    Importer importer(argv[1], [=](const std::string& file) { return file == match; });
    try
    {
        test(importer);
    }
    catch (const std::exception& exc)
    {
        std::cerr << exc.what() << std::endl;
    }
}
