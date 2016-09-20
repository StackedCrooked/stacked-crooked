#include <boost/container/flat_map.hpp>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cassert>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <thread>

// BASIC ALGORITHM
// ---------------
// The idea is to work "pull"-based rather than "push"-based: the PhysicalInterface gets current
// timestamp and "pulls" the ready-to-transmit packets from its BBInterfaces. Each BBInterface pulls
// ready-to-transmit packets from each of its flows.
//
// See:
//  - PhysicalInterface::run
//  - BBBInterface::pull
//  - Flow::pull
//
//
// NOTE:
// The concept of "out-of-resources" is ignored here. It is to be implemented in higher layer
// application logic. If needed, it can also be implemented in PhysicalInterface or BBInterface.
//
//
// TEST SETUP
// ----------
// - A single PhysicalInterface with 100 BBInterfaces connected (trunking, 1Gbit/s).
// - The PhysicalInterface can do 100Gbit/s.
// - Each BBInterface is rate limited to 1Gbit/s.
// - Each BBInterface has 5 flows that each send one fixed-size packet at a fixed rate.
//  -> So there are 500 flows in total.


using Clock = std::chrono::steady_clock;


struct Packet
{
    std::size_t size() const { return mSize; }

    void set_size(std::size_t n)
    {
        mSize = n;
        mData.resize(n);
    }

private:
    std::size_t mSize = 0;
    std::string mData;
};



struct BBInterface;


// Simplified implementation for a BB flow.
struct Flow
{
    void set_bitrate(int64_t mbps)
    {
        mBytesPerSecond = (1e6 * mbps) / 8.0;
        update_frame_interval();
    }

    void set_packet_size(std::size_t packet_size)
    {
        mPacket.set_size(packet_size);
        update_frame_interval();
    }

    void pull(std::vector<Packet*>& packets, Clock::time_point current_time)
    {
        if (mNextTransmission == Clock::time_point{}) // first iteration
        {
            mNextTransmission = current_time;
        }

        if (current_time >= mNextTransmission + 3 * mFrameInterval)
        {
            packets.resize(packets.size() + 4, &mPacket);
            mNextTransmission += 4 * mFrameInterval;
            return;
        }

        if (current_time >= mNextTransmission)
        {
            packets.push_back(&mPacket);
            mNextTransmission += mFrameInterval;
        }
    }

private:
    void update_frame_interval()
    {
        mFrameInterval = std::chrono::nanoseconds(int64_t(1e9 * mPacket.size() / mBytesPerSecond));
    }

    Packet mPacket;
    double mBytesPerSecond = 1e9 / 8;
    Clock::time_point mNextTransmission{};
    std::chrono::nanoseconds mFrameInterval{};
};


struct BBInterface
{
    BBInterface()
    {
        mFlows.reserve(32);
    }

    void set_rate_limit(double bytes_per_s)
    {
        mBytesPerSecond = bytes_per_s;
    }

    Flow& add_flow()
    {
        mFlows.resize(mFlows.size() + 1);
        return mFlows.back();
    }

    void pull(std::vector<Packet*>& packets, Clock::time_point current_time)
    {
        for (auto i = 0; i != 4; ++i)
        {
            if (mQueue.empty())
            {
                for (Flow& flow : mFlows)
                {
                    flow.pull(mQueue, current_time);
                }

                if (mQueue.empty())
                {
                    return;
                }

                std::reverse(mQueue.begin(), mQueue.end());
                //printf("== %d\n", (int)mQueue.size());
            }


            // Apply rate limit.
            if (mBytesPerSecond && mBucket < 0)
            {
                if (mLastTime == Clock::time_point())
                {
                    mLastTime = current_time;
                }
                std::chrono::nanoseconds elapsed_ns = current_time - mLastTime;

                auto bucket_increment = elapsed_ns.count() * mBytesPerSecond / 1e9;
                auto new_bucket = std::min(mBucket + bucket_increment, mMaxBucketSize);
                if (new_bucket < 0)
                {
                    return;
                }

                mBucket = new_bucket;
                mLastTime = current_time;
            }


            Packet* next_packet = mQueue.back();
            auto packet_size = next_packet->size();

            packets.push_back(next_packet);
            mTxBytes += packet_size;
            mQueue.pop_back();

            if (mBytesPerSecond)
            {
                mBucket -= packet_size;
            }
        }
    }

    double mBytesPerSecond = 1e9 / 8;
    double mMaxBucketSize = 16 * 1024;
    double mBucket = mMaxBucketSize;
    Clock::time_point mLastTime = Clock::time_point();
    std::vector<Packet*> mQueue;
    int64_t mTxBytes = 0;
    std::vector<Flow> mFlows;
};


struct Socket
{
    void send_batch(Clock::time_point ts, const std::vector<Packet*>& packets)
    {
        auto total_size = 0ul;
        for (auto& p : packets)
        {
            auto size = p->size();
            assert(size > 0);
            total_size += size;
            mSizes[size]++;
        }

        mTxBytes += total_size;

        if (mTimestamp == Clock::time_point())
        {
            mTimestamp = Clock::now();
        }

        std::chrono::nanoseconds elapsed_ns = ts - mTimestamp;

        if (elapsed_ns >= std::chrono::seconds(1))
        {
            printf("=== Stats ===\n");
            printf("elapsed_ns=%lld TxBytes=%llu AvgRage=%f Gbps\nCounters:\n", elapsed_ns.count(), mTxBytes, 8.0 * mTxBytes / elapsed_ns.count());

            mTxBytes = 0;
            mTimestamp = ts;
            for (auto& el : mSizes)
            {
                printf("  %d: %d\n", (int)el.first, (int)el.second);
            }
            printf("\n");
            mSizes.clear();
        }
    }

    uint64_t mTxBytes = 0;
    Clock::time_point mTimestamp = Clock::time_point();
    boost::container::flat_map<int, int> mSizes;
};


struct PhysicalInterface
{
    PhysicalInterface(std::size_t num_interfaces) :
        mBBInterfaces(num_interfaces),
        mThread()
    {
    }

    PhysicalInterface(const PhysicalInterface&) = delete;
    PhysicalInterface& operator=(const PhysicalInterface&) = delete;

    ~PhysicalInterface()
    {
        stop();
    }

    void start()
    {
        mThread = std::thread(&PhysicalInterface::run_thread, this);
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
        std::vector<Packet*> packets;

        auto now = Clock::now();

        while (!mQuit)
        {
            {
                std::lock_guard<std::mutex> lock(mMutex);
            
                for (BBInterface& bbinterface : mBBInterfaces)
                {
                    bbinterface.pull(packets, now);
                }

                for (BBInterface& bbinterface : mBBInterfaces)
                {
                    bbinterface.pull(packets, now);
                }
            }

            if (!packets.empty())
            {
                mSocket.send_batch(now, packets);
                packets.clear();
            }

            now = Clock::now();
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
    enum { num_interfaces = 100 };

    PhysicalInterface physicalInterface(num_interfaces);


    enum { num_flows_per_interface = 5 };

    int sizes[num_flows_per_interface] = {   64, 128, 256, 512, 1024 };
    int rates[num_flows_per_interface] = {  200, 200, 200, 200,  200 }; // Mbit/s

    static_assert(sizeof(sizes) == sizeof(sizes[0]) * num_flows_per_interface, "");
    static_assert(sizeof(rates) == sizeof(rates[0]) * num_flows_per_interface, "");


    for (auto interface_id = 0; interface_id != num_interfaces; ++interface_id)
    {
        BBInterface& bbInterface = physicalInterface.getBBInterfaces().at(interface_id);
        for (auto flow_id = 0; flow_id != num_flows_per_interface; ++flow_id)
        {
            Flow& flow = bbInterface.add_flow();
            flow.set_packet_size(sizes[flow_id]);
            flow.set_bitrate(rates[flow_id]);
        }
    }

    printf("Number of interfaces: %d\n", int(num_interfaces));
    printf("Total number of flows: %d\n", int(num_interfaces * num_flows_per_interface));


    physicalInterface.start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

