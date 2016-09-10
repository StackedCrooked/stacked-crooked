#include <boost/container/flat_set.hpp>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <deque>
#include <thread>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>


struct Packet
{
    explicit Packet(int flowId) : mFlowId(flowId) {}
    std::size_t size() const { return mData.size(); }
    std::string mData;
    int mFlowId = 0;
};

using Duration  = std::chrono::milliseconds;
using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;


Timestamp gStartTime = Clock::now();



struct BBInterface;



struct Flow
{
    Flow() :
        mPacket([]{ static int flowId; return flowId++; }())
    {
    }

    void consume(std::deque<Packet*>& packets, Timestamp current_time)
    {
        for (auto i = 0; i != 1; ++i)
        {
            if (mPacketCount == 0)
            {
                return;
            }
            if (current_time >= mNextTransmission)
            {   
                packets.push_back(&mPacket);
                mTxBytes += mPacket.size();
                mPacketCount--;
                mNextTransmission += mInterval;
            }
        }
    }
    
    Packet mPacket;
    Timestamp mNextTransmission;
    Duration mInterval;
    int64_t mTxBytes = 0;
    int64_t mPacketCount = 400;
};


struct BBInterface
{
    void set_rate_limit(double bytes_per_ns)
    {
        mBytesPerMs = bytes_per_ns;
    }

    void add_flow(Flow& flow)
    {
        mFlows.insert(&flow);
    }

    void remove_flow(Flow& flow)
    {
        mFlows.erase(&flow);
    }

    void consume(std::deque<Packet*>& packets, Timestamp current_time, int max_packets = 1)
    {
        for (auto i = 0; i != max_packets; ++i)
        {
            if (mPackets.empty())
            {
                for (Flow* flow : mFlows)
                {
                    flow->consume(mPackets, current_time);
                }
                if (mPackets.empty())
                {
                    return;
                }
            }

            Packet* front_packet = mPackets.front();
            auto packet_size = front_packet->size();

            if (mBytesPerMs && mBucket < 0)
            {
                if (mLastTime == Timestamp())
                {
                    mLastTime = current_time;
                }
                std::chrono::milliseconds elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - mLastTime);

                auto bucket_increment = elapsed_ms.count() * mBytesPerMs;
                auto new_bucket = std::min(mBucket + bucket_increment, mMaxBucketSize);
                if (new_bucket < packet_size)
                {
                    return;
                }

                mBucket = new_bucket;
                mLastTime = current_time;
            }


            packets.push_back(front_packet);
            mTransmittedPackets.emplace_back(std::chrono::duration_cast<std::chrono::milliseconds>(current_time - gStartTime), packet_size);
            mTxBytes += packet_size;
            mPackets.pop_front();

            if (mBytesPerMs)
            {
                mBucket -= packet_size;
            }
        }
    }

    void print(int index)
    {
        for (auto& el : mTransmittedPackets)
        {
            std::cout << "BB" << index << ' ' << std::get<0>(el).count() << ' ' << std::get<1>(el) << '\n';
        }
    }

    double mBytesPerMs = 10;
    double mMaxBucketSize = 4 * 1024;
    double mBucket = mMaxBucketSize;
    Timestamp mLastTime = Timestamp();
    boost::container::flat_set<Flow*> mFlows;
    std::deque<Packet*> mPackets;
    std::deque<std::tuple<std::chrono::milliseconds, int32_t>> mTransmittedPackets;
    int64_t mTxBytes = 0;
};




struct Socket
{
    void send_batch(Timestamp ts, const std::deque<Packet*>& packets)
    {
        assert(!packets.empty());
        if (!mDirty)
        {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ts - gStartTime);
            std::cout << std::setw(5) << ms.count() << "ms: ";
        }
        for (Packet* p : packets)
        {
            std::cout << p->size() << ' ';
        }
        mDirty = true;
    }

    void flush()
    {
        if (mDirty)
        {
            std::cout << std::endl;
            mDirty = false;
        }
    }

    bool mDirty = false;
};


struct PhysicalInterface
{
    PhysicalInterface(std::size_t num_interfaces) :
        mBBInterfaces(num_interfaces),
        mThread(&PhysicalInterface::run_thread, this)
    {
    }
    
    PhysicalInterface(const PhysicalInterface&) = delete;
    PhysicalInterface& operator=(const PhysicalInterface&) = delete;
    
    ~PhysicalInterface()
    {
        stop();
    }

    void stop()
    {
        if (!mQuit.exchange(true))
        {
            mThread.join();
        }
    }

    std::vector<BBInterface>& getBBInterfaces()
    {
        return mBBInterfaces;
    }

private:
    void run_thread()
    {
        std::deque<Packet*> packets;
        
        auto now = Clock::now();
        while (!mQuit)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            

            for (BBInterface& bbinterface : mBBInterfaces)
            {
                bbinterface.consume(packets, now);
            }

            if (!packets.empty())
            {
                mSocket.send_batch(now, packets);
                packets.clear();
            }
            else
            {
                now += std::chrono::milliseconds(10);
                mSocket.flush();
            }
        }
    }
    
    std::atomic<bool> mQuit{false};
    std::vector<BBInterface> mBBInterfaces;
    Socket mSocket;
    std::mutex mMutex;
    std::thread mThread;
};


int main()
{
    PhysicalInterface physicalInterface(48);
    BBInterface* bbInterfaces = physicalInterface.getBBInterfaces().data();

    enum { num_flows = 1514 / 64 };
    Flow flows[num_flows];

    auto now = Clock::now();

    for (auto i = 0; i != num_flows; ++i)
    {
        Flow& flow = flows[i];
        flow.mPacket.mData.resize((i + 1) * 64);
        flow.mInterval = std::chrono::milliseconds(10);
        flow.mNextTransmission = now + flow.mInterval;
        bbInterfaces[0].add_flow(flow);
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    physicalInterface.stop();

//    for (BBInterface& b : physicalInterface.getBBInterfaces())
//    {
//        b.print(&b - &physicalInterface.getBBInterfaces()[0]);
//    }
}
