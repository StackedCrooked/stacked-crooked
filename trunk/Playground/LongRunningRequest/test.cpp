//#include <iostream>


//struct Server
//{
//    void handle(const std::string & inRequest, std::string & outResponse)
//    {
//        outResponse = "(" + inRequest + ")";
//    }
//};


//struct Bridge
//{
//    Bridge(Server & inServer) : server(&mServer) {}

//    void handle(const std::string &, std::string & outResponse)
//    {
//        while (!mQuit)
//        {
//            std::unique_lock<std::mutex> lock(mMutex);
//            mCondition.wait(lock);
//            outResponse = std::move(mRequest);
//        }
//    }


//    Server & mServer;
//    std::string mRequest;
//};


//int main()
//{

//}
