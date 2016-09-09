#include <atomic>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <thread>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>


using Packet = std::string;

using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;
using Duration  = std::chrono::nanoseconds;


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
    void send_batch(Timestamp ts, const std::vector<Packet*>& packets)
    {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(ts - mStartTime);
        std::cout << us.count() << ' ';
        for (Packet* p : packets)
        {
            std::cout << *p << " ";
        }
        std::cout << std::endl;
    }
    Clock::time_point mStartTime = Clock::now();
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
            
            auto now = Clock::now();
            
            for (Flow* flow : mFlows)
            {
                flow->consume(packets, now);
            }

            if (!packets.empty())
            {
                mSocket.send_batch(now, packets);
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


enum
{
    num_flows = 4
};






int main()
{
    Transmitter tx;

    std::vector<Flow> flows(num_flows);

    
    auto now = Clock::now();
    for (auto i = 0; i != num_flows; ++i)
    {
        Flow& flow = flows[i];
        flow.mPacket.push_back('A' + i);
        flow.mInterframeGap = std::chrono::microseconds(200 + 100 * i);
        flow.mNextTransmission = now + flows[i].mInterframeGap;
        tx.add_flow(flow);
    }


    std::this_thread::sleep_for(std::chrono::seconds(1));
    

}

