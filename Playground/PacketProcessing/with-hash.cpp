#ifndef PREFETCH
#error "PREFETCH is not defined."
#endif


#ifndef FILTERTYPE
#error "FILTERTYPE is not defined."
#endif

#include "Utils.h"
#include "Networking.h"
#include "NativeFilter.h"
#include "VectorFilter.h"
#include "BPFFilter.h"
#include "MaskFilter.h"
#include "Packet.h"
#include "PCAPWriter.h"
#include <boost/functional/hash.hpp>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>


template<typename FilterType>
struct FlowImpl
{
    FlowImpl(uint8_t protocol, IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port) :
        mFilter(protocol, source_ip, target_ip, src_port, dst_port),
        mHash(0)
    {
        boost::hash_combine(mHash, protocol);
        boost::hash_combine(mHash, source_ip.toInteger());
        boost::hash_combine(mHash, target_ip.toInteger());
        boost::hash_combine(mHash, src_port);
        boost::hash_combine(mHash, dst_port);
    }

    bool match(const uint8_t* frame_bytes, int len) const
    {
        // avoid unpredictable branch here.
        return mFilter.match(frame_bytes, len);
    }

    std::size_t hash() const
    {
        return mHash;
    }

private:
    FilterType mFilter;
    std::size_t mHash;
};


using Flow = FlowImpl<FILTERTYPE>;


struct Bucket
{
    Bucket() :
        mBuffer(),
        mBegin(&mBuffer[0]),
        mEnd(mBegin),
        mVector()
    {
    }

    void push_back(uint16_t value);

    uint16_t operator[](std::size_t i) const { return mBegin[i]; }

    const uint16_t* begin() const { return mBegin; }
    const uint16_t* end() const { return mEnd; }

    bool empty() const { return mBegin == mEnd; }

    std::size_t size() const { return mEnd - mBegin; }
private:
    std::array<uint16_t, 4> mBuffer;
    uint16_t* mBegin;
    uint16_t* mEnd;
    std::unique_ptr<std::vector<uint16_t>> mVector;
};


struct Flows
{
    bool empty()
    {
        return mFlows.empty();
    }

    void add_flow(uint8_t protocol, IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port)
    {
        auto flow_index = mFlows.size();
        mFlows.emplace_back(protocol, source_ip, target_ip, src_port, dst_port);
        auto hash = mFlows[flow_index].hash();
        auto bucket_index = hash % mHashTable.size();

        Bucket& bucket = mHashTable[bucket_index];

        bucket.push_back(flow_index);

        if (bucket.size() == 1)
        {
            mUsedBuckets++;
        }
        if (bucket.size() == 2)
        {
            mUsedBuckets--;
            mSharedBuckets++;
        }
        else if (bucket.size() == 5)
        {
            mSharedBuckets--;
            mOverflowingBuckets++;
        }
    }

    void print()
    {
        std::cout << " " << mUsedBuckets << "/" << mSharedBuckets << "/" << mOverflowingBuckets << std::endl;
    }

    std::size_t size() const
    {
        return mFlows.size();
    }

    void match(const Packet& packet, uint64_t* matches)
    {
        auto ip4_header = Decode<IPv4Header>(packet.data() + sizeof(EthernetHeader));
        auto tcp_header = Decode<TCPHeader>(packet.data() + sizeof(EthernetHeader) + sizeof(IPv4Header));

        std::size_t packet_hash = 0;
        boost::hash_combine(packet_hash, ip4_header.mProtocol);
        boost::hash_combine(packet_hash, ip4_header.mSourceIP.toInteger());
        boost::hash_combine(packet_hash, ip4_header.mDestinationIP.toInteger());
        boost::hash_combine(packet_hash, tcp_header.mSourcePort.hostValue());
        boost::hash_combine(packet_hash, tcp_header.mDestinationPort.hostValue());

        auto bucket_index = packet_hash % mHashTable.size();

        Bucket& flow_indexes = mHashTable[bucket_index];
        for (const uint32_t& flow_index : flow_indexes)
        {
            if (mFlows[flow_index].match(packet.data(), packet.size()))
            {
                matches[flow_index]++;
                break;
            }
        }
    }

    std::vector<Flow> mFlows;

    static_assert(sizeof(Bucket) == 32, "");

    std::array<Bucket, 2048> mHashTable;
    uint32_t mUsedBuckets = 0;
    uint32_t mSharedBuckets = 0;
    uint32_t mOverflowingBuckets = 0;
};


template<uint32_t prefetch>
void run3(std::vector<Packet>& packets, Flows& flows, uint64_t* const matches)
{
    const uint32_t num_flows = flows.size();
    const uint32_t num_packets = packets.size();
    const auto start_time = Clock::now();

    for (auto i = 0ul; i != num_packets; ++i)
    {
        Packet& packet = packets[i];

        if (prefetch > 0)
        {
            __builtin_prefetch(packets[i + prefetch].data() + sizeof(EthernetHeader) + sizeof(IPv4Header), 0, 0);
        }

        flows.match(packet, matches);
    }


    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start_time).count();
    auto ns_per_packet = 1.0 * elapsed_ns / num_packets;
    auto mpps = (1e9 / ns_per_packet) / 1e6;
    auto mpps_rounded = int(0.5 + 100 * mpps)/100.0;

    std::cout << std::setw(12) << std::left << GetTypeName<FILTERTYPE>()
            << " PREFETCH=" << prefetch
            << " FLOWS=" << std::setw(5) << std::left << num_flows
            << " MPPS=" << std::setw(9) << std::left << mpps_rounded
            << " BUCKETS(perfect/shared/overflowing)=" << flows.mUsedBuckets << "/" << flows.mSharedBuckets << "/" << flows.mOverflowingBuckets;

    #if 1
    std::cout << " (verify-matches:";
    for (auto i = 0ul; i != std::min(num_flows, 10u); ++i)
    {
        if (i > 0) std::cout << ',';
        std::cout << int(100000.0 * matches[i] / packets.size()) / 100000.0;
    }
    if (num_flows > 20)
    {
        std::cout << "...";
    }
    std::cout << ")";
    #endif

}



template<int prefetch>
void do_run(uint32_t num_packets, uint32_t num_flows)
{
    std::vector<Packet> packets;
    packets.reserve(num_packets);

    Flows flows;
    flows.mFlows.reserve(num_flows);

    const IPv4Address src_ip(1, 1, 1, 1);
    const IPv4Address dst_ip(1, 1, 1, 1);


    for (auto i = 1ul; i <= num_packets; ++i)
    {
        uint16_t src_port = i % num_flows;
        uint16_t dst_port = i % num_flows;
        packets.emplace_back(6, src_ip, dst_ip, src_port, dst_port);
    }

    for (auto i = 1ul; i <= num_flows; ++i)
    {
        uint16_t src_port = i % num_flows;
        uint16_t dst_port = i % num_flows;
        flows.add_flow(6, src_ip, dst_ip, src_port, dst_port);
    }

    std::random_shuffle(packets.begin(), packets.end());

    std::vector<uint64_t> matches(num_flows);
    run3<prefetch>(packets, flows, matches.data());

    std::cout << std::endl;
}


void run(uint32_t num_packets = 500 * 1000)
{
    int flow_counts[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

    for (auto flow_count : flow_counts)
    {
        do_run<PREFETCH>(num_packets, flow_count);
    }
    std::cout << std::endl;
}


int main()
{
    run();
    std::cout << std::endl;
}


void Bucket::push_back(uint16_t value)
{
    uint32_t length = mEnd - mBegin;

    if (length < mBuffer.size())
    {
        *mEnd++ = value;
        return;
    }

    if (mVector)
    {
        mVector->push_back(value);
        mBegin = mVector->data();
        mEnd = mBegin + mVector->size();
        return;
    }

    mVector.reset(new std::vector<uint16_t>);
    mVector->reserve(2 * mBuffer.size());
    mVector->assign(mBuffer.begin(), mBuffer.end());
    mVector->push_back(value);
    mBegin = mVector->data();
    mEnd = mBegin + mVector->size();
}
