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


// 40/100 Gbit/s
// - enable batching (eth_dev_tx_burst)
// - allow small bursts but avoid cubro loss
// - ensure fairness
// - push vs pull
// - avoid overhead of callback-based approach (HRTimer)
// - amortise overhead of mutex locking and getting system time


// Sample program is very simplified:
// - no stream or frame modifiers
// - one frame per flow
// - single thread
// - fake packet
// - fake socket


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
    }

private:
    std::size_t mSize = 0;
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
            mCounters[size]++;
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
            printf("elapsed_ns=%ld TxBytes=%ld Rate=%f Gbps\nCounters:\n", long(elapsed_ns.count()), long(mTxBytes), 8.0 * mTxBytes / elapsed_ns.count());

            mTxBytes = 0;
            mTimestamp = ts;
            for (auto& el : mCounters)
            {
                auto bitrate = el.first * el.second * 8;
                printf("  %4ld bytes * %8ld => %10ld bps\n", (long)el.first, (long)el.second, (long)bitrate);
            }
            printf("\n");
            mCounters.clear();
        }
    }

    uint64_t mTxBytes = 0;
    Clock::time_point mTimestamp = Clock::time_point();
    boost::container::flat_map<int64_t, int64_t> mCounters;
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

    void set_start_time(Clock::time_point start_time)
    {
        mNextTransmission = start_time;
    }

    void pull(std::deque<Packet*>& packets, Clock::time_point current_time)
    {
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
    void pull(std::vector<Packet*>& packets, Clock::time_point current_time)
    {
        if (mAvailablePackets.empty())
        {
            // Pull a packet from each flow that wants to transmit.
            for (Flow& flow : mFlows)
            {
                flow.pull(mAvailablePackets, current_time);
            }

            if (mAvailablePackets.empty())
            {
                return;
            }
        }

        // Apply rate limit.
        if (is_rate_limited() && mBucketSize < 0)
        {
            std::chrono::nanoseconds elapsed_ns = current_time - mLastUpdate;

            auto bucket_increment = elapsed_ns.count() * mBytesPerSecond / 1e9;
            auto new_bucket = std::min(mBucketSize + bucket_increment, mMaxBucketSize);
            if (new_bucket < 0)
            {
                return;
            }

            mBucketSize = new_bucket;
            mLastUpdate = current_time;
        }

        Packet* next_packet = mAvailablePackets.front();
        mAvailablePackets.pop_front();

        packets.push_back(next_packet);

        mTxBytes += next_packet->size();

        if (is_rate_limited())
        {
            mBucketSize -= next_packet->size();
        }
    }

    bool is_rate_limited() const
    {
        return mBytesPerSecond != 0;
    }

    void set_rate_limit(double bytes_per_s)
    {
        mBytesPerSecond = bytes_per_s;
    }

    Flow& add_flow()
    {
        mFlows.resize(mFlows.size() + 1);
        mFlows.shrink_to_fit();
        return mFlows.back();
    }

private:
    double mBytesPerSecond = 1e9 / 8;
    double mMaxBucketSize = 8 * 1024;
    double mBucketSize = 0;
    Clock::time_point mLastUpdate = Clock::time_point();
    std::deque<Packet*> mAvailablePackets;
    int64_t mTxBytes = 0;
    std::vector<Flow> mFlows;
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
        std::vector<Packet*> packets; // memory is reused every time

        while (!mQuit)
        {
            auto now = Clock::now();

            for (BBInterface& bbinterface : mBBInterfaces)
            {
                bbinterface.pull(packets, now);
            }

            if (!packets.empty())
            {
                mSocket.send_batch(now, packets);
                packets.clear();
            }
        }
    }

    std::atomic<bool> mQuit{false};
    std::vector<BBInterface> mBBInterfaces;
    Socket mSocket;
    std::thread mThread;
};


int main()
{
    enum
    {
        num_interfaces = 100,
        num_flows = 5
    };

    int sizes[num_flows] = {   64, 128, 256, 512, 1024 };
    int rates[num_flows] = {  200, 200, 200, 200,  200 }; // Mbit/s

    static_assert(sizeof(sizes) == sizeof(sizes[0]) * num_flows, "");
    static_assert(sizeof(rates) == sizeof(rates[0]) * num_flows, "");

    auto start_time = Clock::now() + std::chrono::seconds(1);

    PhysicalInterface physicalInterface(num_interfaces);

    for (BBInterface& bbInterface : physicalInterface.getBBInterfaces())
    {
        for (auto i = 0; i != num_flows; ++i)
        {
            Flow& flow = bbInterface.add_flow();
            flow.set_packet_size(sizes[i]);
            flow.set_bitrate(rates[i]);
            flow.set_start_time(start_time);
        }
    }

    printf("Number of interfaces: %d\n", int(num_interfaces));
    printf("Total number of flows: %d\n", int(num_interfaces * num_flows));


    physicalInterface.start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

