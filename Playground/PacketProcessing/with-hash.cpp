﻿#include "Utils.h"
#include "Networking.h"
#include "NativeFilter.h"
#include "VectorFilter.h"
#include "BPFFilter.h"
#include "MaskFilter.h"
#include "Packet.h"
#include "PCAPWriter.h"
#include <boost/functional/hash.hpp>
#include <iomanip>
#include <iostream>
#include <vector>


template<typename FilterType>
struct Flow
{
    Flow(uint8_t protocol, IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port) :
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


template<typename FlowType>
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
        mHashTable[bucket_index].push_back(flow_index);

        #if 0
        if (mHashTable[bucket_index].size() >= 2)
        {
            std::cout << "flow_index=" << flow_index << " hash=" << hash << " bucket_index=" << bucket_index << " bucket_entries=" << mHashTable[bucket_index].size() << std::endl;
        }
        #endif
    }

	void print()
	{
		std::cout << "HashTable:" << std::endl;
		for (auto i = 0u; i != mHashTable.size(); ++i)
		{
			std::cout << " i=" << i << " size=" << mHashTable[i].size() << std::endl;
		}
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
        boost::hash_combine(packet_hash, tcp_header.mSourcePort);
        boost::hash_combine(packet_hash, tcp_header.mDestinationPort);

        auto bucket_index = packet_hash % mHashTable.size();
		//std::cout << "packet_hash=" << packet_hash << " bucket_index=" << bucket_index << std::endl;

        Bucket& flow_indexes = mHashTable[bucket_index];
        if (flow_indexes.empty())
        {
            std::cout << "Packet does not match a flow index." << std::endl;
        }
        for (const uint32_t& flow_index : flow_indexes)
        {
            matches[flow_index] += mFlows[flow_index].match(packet.data(), packet.size());
        }
    }

    std::vector<Flow<FlowType>> mFlows;

    struct Bucket
    {
        using value_type = uint16_t; // number of flows shouldn't exceed 2**16

        Bucket() :
            mBuffer(),
            mBegin(&mBuffer[0]),
            mEnd(mBegin),
            mVector()
        {
        }

        void push_back(value_type value)
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

            mVector.reset(new std::vector<value_type>);
            mVector->reserve(2 * mBuffer.size());
            mVector->assign(mBuffer.begin(), mBuffer.end());
            mVector->push_back(value);
            mBegin = mVector->data();
            mEnd = mBegin + mVector->size();
        }

        value_type operator[](std::size_t i) const { return mBegin[i]; }

        const value_type* begin() const { return mBegin; }
        const value_type* end() const { return mEnd; }

        bool empty() const { return mBegin == mEnd; }

        std::size_t size() const { return mEnd - mBegin; }
    private:
        std::array<value_type, 4> mBuffer;
        value_type* mBegin;
        value_type* mEnd;
        std::unique_ptr<std::vector<value_type>> mVector;
    };

    static_assert(sizeof(Bucket) == 32, "");

    // Using a prime-number because it greatly reduces the number of hash collisions.
    std::array<Bucket, 1021> mHashTable;
};


template<typename FilterType, uint32_t prefetch>
void run3(std::vector<Packet>& packets, Flows<FilterType>& flows, uint64_t* const matches)
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

    std::cout << std::setw(12) << std::left << GetTypeName<FilterType>()
            << " PREFETCH=" << prefetch
            << " FLOWS=" << std::setw(4) << std::left << num_flows
            << " MPPS=" << std::setw(9) << std::left << mpps_rounded
            << " ns_per_packet=" << ns_per_packet
            ;

    #if 1
    std::cout << " (verify-matches:";
    for (auto i = 0ul; i != std::min(num_flows, 20u); ++i)
    {
        if (i > 0) std::cout << ',';
        std::cout << 1.0 * matches[i] / packets.size();
    }
    if (num_flows > 20)
    {
        std::cout << "...";
    }
    std::cout << ")";
    #endif

}



template<typename FilterType, int prefetch>
void do_run(uint32_t num_packets, uint32_t num_flows)
{
    std::vector<Packet> packets;
    packets.reserve(num_packets);

    Flows<FilterType> flows;
    flows.mFlows.reserve(num_flows);

    const IPv4Address src_ip(1, 1, 1, 1);
    const IPv4Address dst_ip(1, 1, 1, 1);


    for (auto i = 1ul; i <= num_packets; ++i)
    {
        uint16_t src_port = i % num_flows;
        uint16_t dst_port = i % num_flows;

        //std::cout << "A i=" << i << " num_flows=" << num_flows << " i % num_flows=" << i%num_flows << std::endl;
        packets.emplace_back(6, src_ip, dst_ip, src_port, dst_port);
    }

    for (auto i = 1ul; i <= num_flows; ++i)
    {
        uint16_t src_port = i % num_flows;
        uint16_t dst_port = i % num_flows;
        //std::cout << "B i=" << i << " num_flows=" << num_flows << " i % num_flows=" << i%num_flows << std::endl;
        flows.add_flow(6, src_ip, dst_ip, src_port, dst_port);
    }

    std::vector<uint64_t> matches(num_flows);
    run3<FilterType, prefetch>(packets, flows, matches.data());
    std::cout << std::endl;
}




template<typename FilterType>
void run(uint32_t num_packets = 100 * 1000)
{
    int flow_counts[] = { 1, 10, 100, 1000 };

    for (auto flow_count : flow_counts)
    {
        do_run<FilterType, 0>(num_packets, flow_count);
    }
    std::cout << std::endl;

    for (auto flow_count : flow_counts)
    {
        do_run<FilterType, 4>(num_packets, flow_count);
    }
    std::cout << std::endl;

    for (auto flow_count : flow_counts)
    {
        do_run<FilterType, 8>(num_packets, flow_count);
    }
    std::cout << std::endl;
}


int main()
{
//    run<BBMaskFilter>();
//    std::cout << std::endl;

    run<MaskFilter>();
    std::cout << std::endl;

    run<BPFFilter>();
    std::cout << std::endl;
}
