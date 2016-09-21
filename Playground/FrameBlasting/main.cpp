#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <thread>
#include <vector>


// - 40/100 Gbit/s
// - enable batching (dpdk api: eth_dev_tx_burst)
// - ensure fairness
// - avoid packet loss (cubro)
// - push vs pull


// Sample program is very simplified:
// - no stream or frame modifiers
// - one frame per flow
// - fake packet
// - fake socket
// - single thread


using Clock = std::chrono::steady_clock;


struct Packet
{
    std::size_t size() const { return mSize; }

    void set_size(std::size_t n) { mSize = n; }

private:
    std::size_t mSize = 0;
};


struct Socket
{
    void send_batch(Clock::time_point ts, const std::vector<Packet*>& packets)
    {
        for (auto& p : packets)
        {
            mTxBytes += p->size();
            mCounters[p->size()]++;
        }

        if (mStartTime == Clock::time_point())
        {
            mStartTime = Clock::now();
        }

        std::chrono::nanoseconds elapsed_ns = ts - mStartTime;

        if (elapsed_ns >= std::chrono::seconds(1))
        {
            printf("\n=== Stats ===\nelapsed_ns=%ld TxBytes=%ld Rate=%f Gbps\nCounters:\n", long(elapsed_ns.count()), long(mTxBytes), 8.0 * mTxBytes / elapsed_ns.count());

            mTxBytes = 0;
            mStartTime = ts;
            for (auto& el : mCounters)
            {
                auto bitrate = el.first * el.second * 8;
                printf("  %4ld bytes * %8ld => %4f Gbit/s\n", (long)el.first, (long)el.second, (double)bitrate/1e9);
            }
            mCounters.clear();
        }
    }

    uint64_t mTxBytes = 0;
    Clock::time_point mStartTime = Clock::time_point();
    std::map<int64_t, int64_t> mCounters;
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
            // Pull a packet from each ready-to-transmit flow.
            for (Flow& flow : mFlows)
            {
                flow.pull(mAvailablePackets, current_time);
            }

            if (mAvailablePackets.empty())
            {
                return;
            }
        }

        if (!check_token_bucket(current_time))
        {
            // We are not allowed to transmit yet.
            return;
        }

        Packet* next_packet = mAvailablePackets.front();
        mAvailablePackets.pop_front();

        packets.push_back(next_packet);

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
    bool check_token_bucket(Clock::time_point current_time)
    {
        if (mBytesPerSecond != 0 && mBucketSize >= 0)
        {
            return true;
        }

        return update_bucket(current_time);
    }

    bool update_bucket(Clock::time_point current_time)
    {
        std::chrono::nanoseconds elapsed_ns = current_time - mLastUpdate;

        auto bucket_increment = elapsed_ns.count() * mBytesPerSecond / 1e9;
        auto new_bucket = std::min(mBucketSize + bucket_increment, mMaxBucketSize);
        if (new_bucket < 0)
        {
            // We must wait a little longer.
            return false;
        }

        mBucketSize = new_bucket;
        mLastUpdate = current_time;
        return true;
    }

    double mBytesPerSecond = 1e9 / 8;
    double mMaxBucketSize = 8 * 1024;
    double mBucketSize = 0;
    Clock::time_point mLastUpdate = Clock::time_point();
    std::deque<Packet*> mAvailablePackets;
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

    auto start_time = Clock::now() + std::chrono::milliseconds(100);

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
