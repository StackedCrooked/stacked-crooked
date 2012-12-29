#include <algorithm>
#include <condition_variable>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>


struct Bridge
{
    void listen(std::string & answer)
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mtx);
            condition.wait_for(lock, std::chrono::seconds(10));
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

    void replace_all(std::string& str, const std::string& oldStr, const std::string& newStr)
    {
      size_t pos = 0;
      while((pos = str.find(oldStr, pos)) != std::string::npos)
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

    std::string join(const std::string & lhs, const std::string & rhs)
    {
        return escape(lhs) + "\t" + escape(rhs);
    }

    std::string MakeRequest(const std::string & command, const std::string & arg)
    {
        return join(command, arg);
    }

    std::future<std::string> delegate(const std::string & command, const std::string & arg)
    {
        std::unique_lock<std::mutex> lock(mtx);
        auto promPtr = std::make_shared<std::promise<std::string> >();
        std::promise<std::string> & prom = *promPtr;
        auto fut = prom.get_future();
        delegated.insert(std::make_pair(MakeRequest(command, arg), promPtr));
        condition.notify_one();
        return fut;
    }

    void finished(const std::string & question, const std::string & answer)
    {
        auto it = delegated.find(question);
        if (it == delegated.end())
        {
            throw std::runtime_error("no matching request");
        }
        std::promise<std::string> & prom = *it->second;
        prom.set_value(answer);
    }

    std::mutex mtx;
    std::condition_variable condition;
    std::map<std::string, std::shared_ptr<std::promise<std::string> > > delegated;
};


struct Server : Bridge
{
    void handle(const std::string & command, const std::string & arg, std::string & answer)
    {
        if (command == "listen")
        {
            listen(answer);
        }
        else if (command == "finished")
        {
            auto sep = arg.find("\t");
            auto question = arg.substr(0, sep);
            auto answer = arg.substr(sep + 1, std::string::npos);
            finished(question, answer);
        }
        else
        {
            this->delegate(command, arg);
        }
    }
};
