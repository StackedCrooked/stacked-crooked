#ifndef BPFEXPRESSION_H
#define BPFEXPRESSION_H


#include "Networking.h"
#include "vectorclass/vectori128.h"
#include <cstdint>
#include <string>


struct Length
{
    explicit Length(int value) : mValue(value) {}

    std::string toString() const;

    int mValue;
};


/**
 * For BPF filters like: "len=128 or len=256 or len=400 ..."
 * Compares a given length to all values and returns true if one of them matches.
 * This can hold up to 8 length values. TODO: support any number of values.
 */
struct Lengths
{
    Lengths() :
        mLengths(0)
    {
    }

    // Very slow. Only do this during configuration time.
    void add(uint16_t length)
    {
        for (auto i = 0; i != mLengths.size(); ++i)
        {
            if (mLengths[i] == 0)
            {
                mLengths.insert(i, length);
                return;
            }
        }

        throw std::runtime_error("Lengths is full.");
    }

	// Creates the vector and calls match_vector
	// Avoid using this method.
	bool match(uint16_t length)
	{
		return match_vector(vec::Vec8s(length));
	}

	// Create the Vec8s from the call-site and then reuse it by calling match_vector on multiple methods.
    bool match_vector(vec::Vec8s length) const
    {
        return horizontal_or(mLengths == length);
    }


    vec::Vec8s mLengths;
};


struct L3Type
{
    explicit L3Type(EtherType ethertype) : mValue(static_cast<uint16_t>(ethertype)) { }

    std::string toString() const;

    Net16 mValue;
};


struct L4Type
{
    explicit L4Type(ProtocolId protocolId) : mValue(protocolId) { }

    std::string toString() const;

    ProtocolId mValue;
};


struct SourceIPv4
{
    explicit SourceIPv4(IPv4Address ip) : mValue(ip) {}

    std::string toString() const;

    IPv4Address mValue;
};


struct DestinationIPv4
{
    explicit DestinationIPv4(IPv4Address ip) : mValue(ip) {}

    std::string toString() const;

    IPv4Address mValue;
};


struct SourceOrDestinationIPv4
{
    explicit SourceOrDestinationIPv4(IPv4Address ip) : mIP(ip) {}

    std::string toString() const;

    IPv4Address mIP;
};


struct SourcePort
{
    explicit SourcePort(uint16_t value) : mValue(value) { }

    std::string toString() const;

    Net16 mValue;
};


struct DestinationPort
{
    explicit DestinationPort(uint16_t value) : mValue(value) { }

    std::string toString() const;

    Net16 mValue;
};


struct SourceOrDestinationPort
{
    explicit SourceOrDestinationPort(uint16_t value) : mValue(value) {}

    std::string toString() const;

    Net16 mValue;
};


#endif // BPFEXPRESSION_H
