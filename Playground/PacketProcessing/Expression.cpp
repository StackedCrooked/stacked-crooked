#include "Expression.h"
#include <iostream>


Expression Expression::True()
{
    Expression result;
    result.mType = Type::True;
    return result;
}


Expression Expression::And(Expression lhs, Expression rhs)
{
    Expression result;
    result.mType = Type::And;
    result.mChildren.push_back(lhs);
    result.mChildren.push_back(rhs);
    return result;
}


Expression Expression::Or(Expression lhs, Expression rhs)
{
    Expression result;
    result.mType = Type::Or;
    result.mChildren.push_back(lhs);
    result.mChildren.push_back(rhs);
    return result;
}


Expression Expression::Length(int value)
{
    Expression result;
    result.mType = Type::Length;
    result.mValue = value;
    return result;
}


Expression Expression::bpf_l3_type(EtherType ethertype)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<L3Type>(ethertype);
    return result;
}


Expression Expression::bpf_l4_type(ProtocolId protocolId)

{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<L4Type>(protocolId);
    return result;
}


Expression Expression::bpf_src_ip(IPv4Address value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<SourceIPv4>(value);
    return result;
}


Expression Expression::bpf_dst_ip(IPv4Address value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<DestinationIPv4>(value);
    return result;
}


Expression Expression::bpf_src_or_dst_ip(IPv4Address value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<SourceOrDestinationIPv4>(value);
    return result;
}


Expression Expression::bpf_src_port(uint16_t value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<SourcePort>(value);
    return result;
}


Expression Expression::bpf_dst_port(uint16_t value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<DestinationPort>(value);
    return result;
}


Expression Expression::bpf_src_or_dst_port(uint16_t value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<SourceOrDestinationPort>(value);
    return result;
}


bool Expression::match(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const
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


void Expression::print(int level) const
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


void Expression::print_true(int level) const
{
    std::cout << indent(level) << "<true/>\n";
}


void Expression::print_length(int level) const
{
    std::cout << indent(level) << "<bpf>len=" << mValue << "</bpf>\n";
}


void Expression::print_bpf(int level) const
{
    std::cout << indent(level) << "<bpf>" << mBPF->toString() << "</bpf>\n";
}


void Expression::print_binary(const char* op, int level) const
{
    std::cout << indent(level) << "<" << op << ">\n";
    for (const Expression& child : mChildren)
    {
        child.print(level + 2);
    }
    std::cout << indent(level) << "</" << op << ">\n";
}


std::string Expression::indent(int level)
{
    std::string result;
    for (auto i = 0; i != level; ++i)
    {
        result.push_back(' ');
    }
    return result;
}
