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

    void set_mbps(int64_t mbps)
    {
        mBytesPerSecond = 1e6 * mbps / 8;
    }

    void consume(std::deque<Packet*>& packets, Timestamp current_time)
    {
        if (mNextTransmission == Timestamp())
        {
            mNextTransmission = current_time;
        }

        while (current_time >= mNextTransmission)
        {
            packets.push_back(&mPacket);
            mTxBytes += mPacket.size();
            mNextTransmission += std::chrono::nanoseconds(1000ul * 1000ul * 1000ul * mPacket.size() / mBytesPerSecond);
        }
    }
    
    Packet mPacket;
    Timestamp mNextTransmission = Timestamp();
    int64_t mBytesPerSecond = 1e9 / 8;
    int64_t mTxBytes = 0;
};


struct BBInterface
{
    void set_rate_limit(double bytes_per_ns)
    {
        mBytesPerNs = bytes_per_ns;
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

            if (mBytesPerNs && mBucket < 0)
            {
                if (mLastTime == Timestamp())
                {
                    mLastTime = current_time;
                }
                std::chrono::nanoseconds elapsed_ns = current_time - mLastTime;

                auto bucket_increment = elapsed_ns.count() * mBytesPerNs / 1e9;
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

            if (mBytesPerNs)
            {
                mBucket -= packet_size;
            }
        }
    }

    double mBytesPerNs = 1e9 / 8;
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
        auto total_size = 0ul;
        for (auto p : packets)
        {
            total_size += p->size();
        }

        mTxBytes += total_size;

        if (mTimestamp == Timestamp())
        {
            mTimestamp = Clock::now();
        }


        std::chrono::nanoseconds elapsed_ns = ts - mTimestamp;

        if (elapsed_ns > std::chrono::seconds(1))
        {
            std::cout << "elapsed_ns=" << elapsed_ns.count() << " TxBytes=" << mTxBytes << " ByteRate=" << int(10 * 8 * 1e3 * mTxBytes / elapsed_ns.count())/10.0 << "Mbps" << std::endl;
            mTxBytes = 0;
            mTimestamp = ts;
        }

    }

    void flush()
    {
    }

    uint64_t mTxBytes = 0;
    Timestamp mTimestamp = Timestamp();
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
        
        while (!mQuit)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            auto now = Clock::now();

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

    enum { num_flows = 1 };
    Flow flows[num_flows];

    for (auto i = 0; i != num_flows; ++i)
    {
        Flow& flow = flows[i];
        flow.mPacket.mData.resize(1514);
        flow.set_mbps(500);
        bbInterfaces[0].add_flow(flow);
    }


    std::this_thread::sleep_for(std::chrono::seconds(10));

    physicalInterface.stop();

//    for (BBInterface& b : physicalInterface.getBBInterfaces())
//    {
//        b.print(&b - &physicalInterface.getBBInterfaces()[0]);
//    }
}
