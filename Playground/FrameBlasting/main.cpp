#include <algorithm>
#include <atomic>
#include <chrono>
#include <cassert>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <thread>


//
// SETUP (imaginary)
// -----
// A single PhysicalInterface with 100 BBInterfaces connected.
// The PhysicalInterface can do 100Gbit/s.
// Each BBInterface is rate limited to 1Gbit/s.
// Each BBInterface has 5 flows that each send one fixed-size packet at a fixed rate.
// So there are 500 flows in total.
//
// BASIC IDEA:
// Pull-based rather than push-based:
// - The PhysicalInterface "pulls" packets from its BBInterface
// - Each BBInterface provides packets from its flows as follows:
//     (1) if queue is empty then we pull one packet from each flow into the queue (fairness)
//     (2) pop packets from the queue using token bucket (rate limiting)
//     (3) if queue becomes empty then step (1) is repeated


using Clock = std::chrono::steady_clock;


struct Packet
{
    std::size_t size() const { return mData.size(); }
    std::string mData;
};



struct BBInterface;


struct Flow
{
    void set_bitrate(int64_t mbps)
    {
        mBytesPerSecond = (1e6 * mbps) / 8.0;
        update_frame_interval();
    }

    void set_packet_size(std::size_t n)
    {
        mPacket.mData.resize(n);
        assert(mPacket.size() > 0);

        update_frame_interval();
    }

    void pull(std::deque<Packet*>& packets, Clock::time_point current_time)
    {
        if (mNextTransmission == Clock::time_point{}) // first iteration
        {
            mNextTransmission = current_time;
        }

        if (current_time >= mNextTransmission)
        {
            packets.push_back(&mPacket);
            mNextTransmission += mInterval;
        }
    }

private:
    void update_frame_interval()
    {
        mInterval = std::chrono::nanoseconds(int64_t(1e9 * mPacket.size() / mBytesPerSecond));
    }

    Packet mPacket;
    double mBytesPerSecond = 1e9 / 8;
    Clock::time_point mNextTransmission{};
    std::chrono::nanoseconds mInterval{};
};


struct BBInterface
{
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


        Packet* front_packet = mQueue.front();
        auto packet_size = front_packet->size();

        packets.push_back(front_packet);
        mTxBytes += packet_size;
        mQueue.pop_front();

        if (mBytesPerSecond)
        {
            mBucket -= packet_size;
        }
    }

    double mBytesPerSecond = 1e9 / 8;
    double mMaxBucketSize = 16 * 1024;
    double mBucket = mMaxBucketSize;
    Clock::time_point mLastTime = Clock::time_point();
    std::deque<Packet*> mQueue;
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
            std::cout << "=== Stats ===\n";
            std::cout << "elapsed_ns=" << elapsed_ns.count() << " TxBytes=" << mTxBytes << " Rate=" << int(10 * 8 * mTxBytes / elapsed_ns.count())/10.0 << "Gbps\nCounters:\n";
            mTxBytes = 0;
            mTimestamp = ts;
            for (auto& el : mSizes)
            {
                std::cout << ' ' << std::setw(5) << el.first << ": " << el.second << '\n';
            }
            std::cout << std::endl;
            mSizes.clear();
        }
    }

    uint64_t mTxBytes = 0;
    Clock::time_point mTimestamp = Clock::time_point();
    std::map<int, int> mSizes;
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
            std::lock_guard<std::mutex> lock(mMutex);

            for (BBInterface& bbinterface : mBBInterfaces)
            {
                bbinterface.pull(packets, now);
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

    std::cout << "Number of interfaces: " << num_interfaces << std::endl;
    std::cout << "Total number of flows: " << num_interfaces * num_flows_per_interface << std::endl;


    physicalInterface.start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
}
