#include <atomic>
#include <cassert>
#include <map>
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


namespace {


struct Packet
{
    std::size_t size() const { return mData.size(); }
    std::string mData;
};

using Duration  = std::chrono::milliseconds;
using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;


Timestamp gStartTime = Clock::now();



struct BBInterface;


struct Flow
{
    void set_mbps(int64_t mbps)
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

    void pull(std::deque<Packet*>& packets, Timestamp current_time)
    {
        if (mNextTransmission == Timestamp{})
        {
            mNextTransmission = current_time;
        }

        for (auto i = 0; i != 2; ++i)
        {
            if (current_time < mNextTransmission)
            {
                break;
            }

            packets.push_back(&mPacket);
            mTxBytes += mPacket.size();

            assert(mPacket.size() > 0);
            mNextTransmission += mInterval;
        }
    }

private:
    void update_frame_interval()
    {
        mInterval = std::chrono::nanoseconds(int64_t((1e9 * mPacket.size()) / mBytesPerSecond));
    }

    Packet mPacket;
    double mBytesPerSecond = 1e9 / 8;
    int64_t mTxBytes = 0;
    Timestamp mNextTransmission{};
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

    void pull(std::vector<Packet*>& packets, Timestamp current_time)
    {
        if (mPackets.empty())
        {
            for (Flow& flow : mFlows)
            {
                flow.pull(mPackets, current_time);
            }

            if (mPackets.empty())
            {
                return;
            }
        }

        Packet* front_packet = mPackets.front();
        assert(front_packet->size() > 0);
        auto packet_size = front_packet->size();

        if (mBytesPerSecond && mBucket < packet_size)
        {
            if (mLastTime == Timestamp())
            {
                mLastTime = current_time;
            }
            std::chrono::nanoseconds elapsed_ns = current_time - mLastTime;

            auto bucket_increment = elapsed_ns.count() * mBytesPerSecond / 1e9;
            auto new_bucket = std::min(mBucket + bucket_increment, mMaxBucketSize);
            if (new_bucket < packet_size)
            {
                return;
            }

            mBucket = new_bucket;
            mLastTime = current_time;
        }


        packets.push_back(front_packet);
        mTxBytes += packet_size;
        mPackets.pop_front();

        if (mBytesPerSecond)
        {
            mBucket -= packet_size;
        }
    }

    double mBytesPerSecond = 1e9 / 8;
    double mMaxBucketSize = 8 * 1024;
    double mBucket = mMaxBucketSize;
    Timestamp mLastTime = Timestamp();
    std::deque<Packet*> mPackets;
    int64_t mTxBytes = 0;
    std::vector<Flow> mFlows;
};


struct Socket
{
    void send_batch(Timestamp ts, const std::vector<Packet*>& packets)
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

        if (mTimestamp == Timestamp())
        {
            mTimestamp = Clock::now();
        }


        std::chrono::nanoseconds elapsed_ns = ts - mTimestamp;

        if (elapsed_ns >= std::chrono::seconds(1))
        {
            std::cout << "elapsed_ns=" << elapsed_ns.count() << " TxBytes=" << mTxBytes << " ByteRate=" << int(10 * 8000 * mTxBytes / elapsed_ns.count())/10.0 << "Mbps" << std::endl;
            mTxBytes = 0;
            mTimestamp = ts;
            for (auto& el : mSizes)
            {
                std::cout << ' ' << std::setw(5) << el.first << ": " << el.second << std::endl;
            }
            mSizes.clear();
        }
    }

    uint64_t mTxBytes = 0;
    Timestamp mTimestamp = Timestamp();
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


}


int main()
{

    enum { num_interfaces = 100 };

    PhysicalInterface physicalInterface(num_interfaces);


    physicalInterface.getBBInterfaces().resize(num_interfaces);
    enum { num_flows = 5 };

    int sizes[num_flows] = {   64, 128, 256, 512, 1024 };
    int mbps[num_flows]  = {  200, 200, 200, 200,  200 };

    static_assert(sizeof(sizes) == sizeof(sizes[0]) * num_flows, "");
    static_assert(sizeof(mbps) == sizeof(mbps[0]) * num_flows, "");


    for (auto interface_id = 0; interface_id != num_interfaces; ++interface_id)
    {
        BBInterface& bbInterface = physicalInterface.getBBInterfaces().at(interface_id);
        for (auto flow_id = 0; flow_id != num_flows; ++flow_id)
        {
            Flow& flow = bbInterface.add_flow();
            flow.set_packet_size(sizes[flow_id]);
            flow.set_mbps(mbps[flow_id]);
        }
    }


    physicalInterface.start();
    std::this_thread::sleep_for(std::chrono::seconds(20));
}
