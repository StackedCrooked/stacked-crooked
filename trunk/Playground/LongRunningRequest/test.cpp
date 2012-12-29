#include <algorithm>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>


struct Server
{
    void handle(const std::string & question, std::string & answer)
    {
        if (question == "flip")
        {
            std::reverse(answer.begin(), answer.end());
        }
        else
        {
            // GIGO
            answer = question;
        }
    }
};


struct Bridge
{
    Bridge(Server & s) : server(&s) {}

    void handle(const std::string & question, std::string & answer)
    {
        if (question == "listen")
        {
            while (true)
            {
                std::unique_lock<std::mutex> lock(mtx);
                condition.wait_for(lock, std::chrono::seconds(10));
                if (!questions.empty())
                {
                    answer = "[" + questions.front() + "]";
                    questions.pop_front();
                }
                else
                {
                    answer = "[]";
                }
            }
        }
        else
        {
            answer = std::move(question);
        }
    }

    void delegate(const std::string & q)
    {
        std::unique_lock<std::mutex> lock(mtx);
        questions.push_back(q);
        condition.notify_one();
    }


    Server * server;
    std::mutex mtx;
    std::condition_variable condition;
    std::deque<std::string> questions;
};


int main()
{

}
