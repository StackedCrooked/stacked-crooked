#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>


typedef std::function<void(const std::string&)> Callback;



void replace_all(std::string & str, const std::string & oldStr, const std::string & newStr)
{
    size_t pos = 0;
    while ((pos = str.find(oldStr, pos)) != std::string::npos)
    {
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}

std::string escape(std::string data)
{
    replace_all(data, "\t", "\\t");
    return data;
}

std::string unescape(std::string data)
{
    replace_all(data, "\\t", "\t");
    return data;
}

std::string join(const std::string & lhs, const std::string & rhs)
{
    return escape(lhs) + "\t" + escape(rhs);
}

std::vector<std::string> split(const std::string & s, char delim = '\t')
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> result;
    while (std::getline(ss, item, delim))
    {
        result.push_back(item);
    }
    return result;
}


struct Bridge
{
    void listen(std::string & answer)
    {
        while (true)
        {
            boost::unique_lock<boost::mutex> lock(mtx);
            condition.wait_for(lock, boost::chrono::seconds(1));
            if (!delegated.empty())
            {
                for (const auto & pair : delegated)
                {
                    const auto & question = pair.first;
                    answer = question;
                    break; // assume only one request for now
                }
            }
            else
            {
                answer = "[]";
            }
        }
    }

    std::string MakeRequest(const std::string & command, const std::string & arg)
    {
        return join(command, arg);
    }

    void delegate(const std::string & command, const std::string & arg, const Callback & callback)
   {
        std::cout << "*** delegate ***" << std::endl;
        boost::unique_lock<boost::mutex> lock(mtx);
        delegated.insert(std::make_pair(MakeRequest(command, arg), callback));
        condition.notify_one();
    }

    void finished(const std::string & question, const std::string & answer)
    {
        auto it = delegated.find(question);
        if (it == delegated.end())
        {
            throw std::runtime_error("no matching request");
        }
        const Callback & cb = it->second;
        cb(answer);
    }

    boost::mutex mtx;
    boost::condition_variable condition;
    std::map<std::string, Callback> delegated;
};


struct Server
{
    std::string handle(const std::string & request)
    {
        std::cout << "Server::handle " << request << std::endl;
        auto parts = split(request, '\t');
        if (parts.empty())
        {
            std::cout << "Invalid request: " << std::endl << request << std::endl;
            throw std::runtime_error("Invalid request.");
        }
        std::string answer;
        handle(unescape(parts[0]), unescape(parts.size() >= 2 ? parts[1] : ""), answer);
        return answer;
    }

    void handle(const std::string & command, const std::string & arg, std::string & answer)
    {
        std::cout << "Server::handle " << command << ", " << arg << std::endl;
        if (command == "listen")
        {
            bridge.listen(answer);
        }
        else if (command == "finished")
        {
            auto sep = arg.find("\t");
            auto question = arg.substr(0, sep);
            auto answer = arg.substr(sep + 1, std::string::npos);
            bridge.finished(question, answer);
        }
        else
        {
            bridge.delegate(command, arg, [&answer](const std::string & ans){
                answer = ans;
            });
        }
    }

    Bridge bridge;
};
