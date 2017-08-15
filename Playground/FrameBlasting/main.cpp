#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <thread>
#include <vector>


// How can we do Frame Blasting at 100 Gbit/s?
// - need very low overhead
// - need batching (but Frame Blasting is packet-based!)

// Frame Blasting Requirements:
// - accurate timing
// - fairness
// - no big bursts in order to avoid packet loss on Cubro

// Sample program:
// - Assume 100 trunking interfaces connected to single 100 Gbit/s physical interface.
// - We configure 5 flows on each trunking interface:
//     . Flow 1: 200 Mbit/s using 64-byte packets
//     . Flow 2: 200 Mbit/s using 128-byte packets
//     . Flow 3: 200 Mbit/s using 256-byte packets
//     . Flow 4: 200 Mbit/s using 512-byte packets
//     . Flow 5: 200 Mbit/s using 1024-byte packets
// - So we have 500 flows with total bitrate of 100 Gbit/s.

// Simplifications:
// - no size or timing modifiers
// - only have one frame per stream/flow
// - fake packet, fake socket
// - no Rx


using Clock = std::chrono::steady_clock;

std::map<int64_t, int64_t> mCounters;

struct Packet
{
    std::size_t size() const { return mSize; }

    void set_size(std::size_t n)
    {
        mSize = n;
        counter = &mCounters[mSize];
    }


    int64_t* counter;

private:
    std::size_t mSize = 0;
};


struct Socket
{
    void send_batch(Clock::time_point ts, const std::vector<Packet*>& packets);

    uint64_t mTxBytes = 0;
    Clock::time_point mStartTime = Clock::time_point();
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


    template<typename F>
    void pull(F f, Clock::time_point current_time)
    {
        for (auto i = 0; i != 3; ++i) // allow getting multiple packets at once (catch-up)
        {
            if (current_time >= mNextTransmission)
            {
                f(mPacket);
                mNextTransmission += mFrameInterval;
            }
            else
            {
                break;
            }
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
    template<typename F>
    void pull(F send_function, Clock::time_point current_time)
    {
        // Pull a packet from each ready-to-transmit flow.
        if (check_token_bucket(current_time))
        {
            for (Flow& flow : mFlows)
            {
                flow.pull([this, send_function](Packet& packet) {
                    mBucketSize -= packet.size();
                    send_function(packet);
                }, current_time);
            }
        }
    }

    bool is_rate_limited() const
    {
        return mBytesPerFourNanoseconds != 0;
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
        if (mBytesPerFourNanoseconds != 0 && mBucketSize >= 0)
        {
            return true;
        }

        return update_bucket(current_time);
    }

    bool update_bucket(Clock::time_point current_time)
    {
        std::chrono::nanoseconds elapsed_ns = current_time - mLastUpdate;

        auto bucket_increment = 4 * elapsed_ns.count() * mBytesPerFourNanoseconds;
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

    int64_t mBytesPerFourNanoseconds = 1;
    int64_t mMaxBucketSize = 32 * 1024;
    int64_t mBucketSize = mMaxBucketSize;
    Clock::time_point mLastUpdate = Clock::time_point();
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
        packets.reserve(32);

        while (!mQuit)
        {
            auto now = Clock::now();

            for (BBInterface& bbinterface : mBBInterfaces)
            {
                bbinterface.pull([&](Packet& packet) {
                    packets.push_back(&packet);
                    ++*packet.counter;
                    if (packets.size() == packets.capacity()) {
                        mSocket.send_batch(now, packets);
                        packets.clear();
                    }
                }, now);
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


void Socket::send_batch(std::chrono::steady_clock::time_point ts, const std::vector<Packet*>& packets)
{
    for (auto& p : packets)
    {
        mTxBytes += p->size();
        //++*p->counter;
        //mCounters[p->size()]++;
    }

    if (mStartTime == Clock::time_point())
    {
        mStartTime = Clock::now();
    }

    std::chrono::nanoseconds elapsed_ns = ts - mStartTime;

    if (elapsed_ns >= std::chrono::seconds(1))
    {
        printf("\n=== Stats ===\nelapsed_ns=%ld TxBytes=%ld Rate=%f Gbit/s\nCounters:\n", long(elapsed_ns.count()), long(mTxBytes), 8.0 * mTxBytes / elapsed_ns.count());

        mTxBytes = 0;
        mStartTime = ts;
        for (auto& el : mCounters)
        {
            auto packet_size = el.first;
            auto packet_rate = el.second;
            el.second = 0;
            auto bitrate = packet_size * packet_rate * 8;
            printf("  %4ld bytes * %8ld => %4f Gbit/s\n", (long)packet_size, (long)packet_rate, (double)bitrate/1e9);
        }
    }
}


int main()
{
    enum
    {
        num_interfaces = 100,
        num_flows = 5
    };

    int sizes[num_flows] = {   64, 128, 256, 512, 1024 };
    int rates[num_flows] = {  500, 500, 500, 500,  500 }; // Mbit/s

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
    std::this_thread::sleep_for(std::chrono::seconds(20));
}
