#ifndef EXPRESSION_H
#define EXPRESSION_H


#include "Networking.h"
#include "Utils.h"
#include <iostream>
#include <string>
#include <vector>


struct BPFExpression
{
    virtual ~BPFExpression() {}

    bool match(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const
    {
        return match_impl(data, size, l3_offset, l4_offset);
    }

    std::string toString() const
    {
        return toStringImpl();
    }

private:
    virtual bool match_impl(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const = 0;
    virtual std::string toStringImpl() const = 0;

};


struct L3Type : BPFExpression
{
    explicit L3Type(EtherType ethertype) :
        mValue(static_cast<uint16_t>(ethertype))
    {
    }

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t /*l3_offset*/, uint32_t /*l4_offset*/) const override final
    {
        return Decode<EthernetHeader>(data).mEtherType == mValue;
    }

    std::string toStringImpl() const override final
    {
        switch (EtherType(mValue.hostValue()))
        {
            case EtherType::IPv4: return "ip";
            case EtherType::IPv6: return "ip6";
            case EtherType::VLAN: return "vlan";
            case EtherType::ARP: return "arp";
        }
        return "(unknown l3)";
    }

    Net16 mValue;
};


struct L4Type : BPFExpression
{
    explicit L4Type(ProtocolId protocolId) :
        mValue(protocolId)
    {
    }

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l4_offset*/) const override final
    {
        return Decode<IPv4Header>(data + l3_offset).mProtocol == mValue;
    }

    std::string toStringImpl() const override final
    {
        switch (mValue)
        {
            case ProtocolId::ICMP: return "icmp";
            case ProtocolId::IGMP: return "igmp";
            case ProtocolId::TCP: return "tcp";
            case ProtocolId::UDP: return "udp";
        }
        return "(unknown l3)";
    }


    ProtocolId mValue;
};


struct SourceIPv4 : BPFExpression
{
    explicit SourceIPv4(IPv4Address ip) : mIP(ip) {}

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l4_offset*/) const override final
    {
        return Decode<IPv4Header>(data + l3_offset).mSourceIP == mIP;
    }

    std::string toStringImpl() const override final
    {
        return "src ip " + mIP.toString();
    }

    IPv4Address mIP;
};


struct DestinationIPv4 : BPFExpression
{
    explicit DestinationIPv4(IPv4Address ip) : mIP(ip) {}

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l4_offset*/) const override final
    {
        return Decode<IPv4Header>(data + l3_offset).mDestinationIP == mIP;
    }

    std::string toStringImpl() const override final
    {
        return "dst ip " + mIP.toString();
    }

    IPv4Address mIP;
};


struct SourceOrDestinationIPv4 : BPFExpression
{
    explicit SourceOrDestinationIPv4(IPv4Address ip) : mIP(ip) {}

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l4_offset*/) const override final
    {
        return Decode<IPv4Header>(data + l3_offset).mSourceIP      == mIP
            || Decode<IPv4Header>(data + l3_offset).mDestinationIP == mIP;
    }

    std::string toStringImpl() const override final
    {
        return "src or dst ip " + mIP.toString();
    }

    IPv4Address mIP;
};


struct SourcePort : BPFExpression
{
    explicit SourcePort(uint16_t value) : mValue(value)
    {
    }

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t /*l3_offset*/, uint32_t l4_offset) const override final
    {
        auto src_port = Decode<TCPHeader>(data + l4_offset).mSourcePort;
        return src_port == mValue;
    }

    std::string toStringImpl() const override final
    {
        return "src port " + std::to_string(mValue.hostValue());
    }

    Net16 mValue;
};


struct DestinationPort : BPFExpression
{
    explicit DestinationPort(uint16_t value) : mValue(value)
    {
    }

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t /*l3_offset*/, uint32_t l4_offset) const override final
    {
        return Decode<TCPHeader>(data + l4_offset).mDestinationPort == mValue;
    }

    std::string toStringImpl() const override final
    {
        return "dst port " + std::to_string(mValue.hostValue());
    }

    Net16 mValue;
};


struct SourceOrDestinationPort : BPFExpression
{
    explicit SourceOrDestinationPort(uint16_t value) : mValue(value) {}

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t /*l3_offset*/, uint32_t l4_offset) const override final
    {
        return Decode<TCPHeader>(data + l4_offset).mSourcePort      == mValue
            || Decode<TCPHeader>(data + l4_offset).mDestinationPort == mValue;
    }

    std::string toStringImpl() const override final
    {
        return "src or dst port " + std::to_string(mValue.hostValue());
    }

    Net16 mValue;
};


struct Expression
{
    static Expression True()
    {
        Expression result;
        result.mType = Type::True;
        return result;
    }
    static Expression And(Expression lhs, Expression rhs)
    {
        Expression result;
        result.mType = Type::And;
        result.mChildren.push_back(lhs);
        result.mChildren.push_back(rhs);
        return result;
    }

    static Expression Or(Expression lhs, Expression rhs)
    {
        Expression result;
        result.mType = Type::Or;
        result.mChildren.push_back(lhs);
        result.mChildren.push_back(rhs);
        return result;
    }

    static Expression Length(int value)
    {
        Expression result;
        result.mType = Type::Length;
        result.mValue = value;
        return result;
    }

    static Expression bpf_l3_type(EtherType ethertype)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = std::make_shared<L3Type>(ethertype);
        return result;
    }

    static Expression bpf_l4_type(ProtocolId protocolId)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = std::make_shared<L4Type>(protocolId);
        return result;
    }

    static Expression bpf_src_ip(IPv4Address value)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = std::make_shared<SourceIPv4>(value);
        return result;
    }

    static Expression bpf_dst_ip(IPv4Address value)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = std::make_shared<DestinationIPv4>(value);
        return result;
    }

    static Expression bpf_src_or_dst_ip(IPv4Address value)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = std::make_shared<SourceOrDestinationIPv4>(value);
        return result;
    }

    static Expression bpf_src_port(uint16_t value)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = std::make_shared<SourcePort>(value);
        return result;
    }

    static Expression bpf_dst_port(uint16_t value)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = std::make_shared<DestinationPort>(value);
        return result;
    }

    static Expression bpf_src_or_dst_port(uint16_t value)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = std::make_shared<SourceOrDestinationPort>(value);
        return result;
    }

    bool match(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const
    {
        switch (mType)
        {
            case Type::True:
            {
                return true;
            }
            case Type::And:
            {
                for (const Expression& child : mChildren)
                {
                    if (!child.match(data, size, l3_offset, l4_offset))
                    {
                        return false;
                    }
                }
                return true;
            }
            case Type::Or:
            {
                for (const Expression& child : mChildren)
                {
                    if (child.match(data, size, l3_offset, l4_offset))
                    {
                        return true;
                    }
                }
                return false;
            }
            case Type::Length:
            {
                if (static_cast<int32_t>(size) != mValue)
                {
                    return false;
                }
                return true;
            }
            case Type::BPF:
            {
                return mBPF->match(data, size, l3_offset, l4_offset);
            }
        }

        throw std::runtime_error("Invalid expression type");
    }

    enum class Type
    {
        True, And, Or, BPF, Length
    };

    void print(int level = 0) const
    {
        switch (mType)
        {
            case Type::True:
            {
                print_true(level);
                break;
            }
            case Type::And:
            {
                print_binary("and", level);
                break;
            }
            case Type::Or:
            {
                print_binary("or", level);
                break;
            }
            case Type::Length:
            {
                print_length(level);
                break;
            }
            case Type::BPF:
            {
                print_bpf(level);
                break;
            }
        }
    }

    void print_true(int level) const;
    void print_length(int level) const;
    void print_bpf(int level) const;
    void print_binary(const char* op, int level) const;

    static std::string indent(int level);

    Type mType = Type::And;
    int mValue = 0;
    std::shared_ptr<BPFExpression> mBPF;
    std::vector<Expression> mChildren;
};

#endif // EXPRESSION_H
