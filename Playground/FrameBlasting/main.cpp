#include <atomic>
#include <algorithm>
#include <thread>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>


using Packet = std::string;
using Timestamp = int64_t;
using Duration  = int64_t;


Timestamp GetTimestamp() { return 0; }


struct Flow
{
    void consume(std::vector<Packet*>& packets, Timestamp current_time)
    {   
        if (current_time >= mNextTransmission)
        {   
            packets.push_back(&mPacket);
            mNextTransmission += mInterframeGap;
        }
    }
    
    Packet mPacket;
    Timestamp mNextTransmission;
    Duration mInterframeGap;
};


struct Socket
{
    void send_batch(const std::vector<Packet*>&) {}
};


struct Transmitter
{
    Transmitter() :
        mThread(&Transmitter::run_thread, this)
    {
    }
    
    Transmitter(const Transmitter&) = delete;
    Transmitter& operator=(const Transmitter&) = delete;
    
    ~Transmitter()
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mQuit = true;
        }
        mThread.join();
    }
    
    void add_flow(Flow& flow)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mFlows.push_back(&flow);
    }
    
    void remove_flow(Flow& flow)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mFlows.erase(std::remove(mFlows.begin(), mFlows.end(), &flow), mFlows.end());
    }

private:
    void run_thread()
    {
        std::vector<Packet*> packets;
        
        while (!mQuit)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            
            auto now = GetTimestamp();
            
            for (Flow* flow : mFlows)
            {
                flow->consume(packets, now);
            }

            if (!packets.empty())
            {
                mSocket.send_batch(packets);
                packets.clear();
            }
        }
    }
    
    std::atomic<bool> mQuit{false};
    std::vector<Flow*> mFlows;
    Socket mSocket;
    std::mutex mMutex;
    std::thread mThread;
};


int main()
{
}
