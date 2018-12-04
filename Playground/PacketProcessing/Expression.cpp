#include "Expression.h"
#include "BPFCompositeExpression.h"
#include <cassert>
#include <iostream>


Expression Expression::True()
{
    Expression result;

    // And expression without children is true.
    result.mType = Type::And;
    return result;
}

Expression Expression::And(Expression lhs, Expression rhs)
{
    Expression result;
    result.mType = Type::And;
    result.and_append(std::move(lhs));
    result.and_append(std::move(rhs));
    if (result.mChildren.empty())
    {
        assert(result.mBPF);
        result.mType = Type::BPF;
    }
    return result;
}

void Expression::and_append(Expression expr)
{
    assert(is_and());

    if (expr.is_and())
    {
        for (Expression& e : expr.mChildren)
        {
            and_append(std::move(e));
        }
    }
    else if (expr.is_bpf())
    {
        if (!mBPF)
        {
            mBPF.reset(new BPFCompositeExpression());
        }

        BPFCompositeExpression& comp = *expr.mBPF;
        mBPF->merge_and(comp);
    }
    else
    {
        mChildren.push_back(std::move(expr));
    }
}


Expression Expression::Or(Expression lhs, Expression rhs)
{
    if (lhs.is_bpf() && rhs.is_bpf())
    {
        if (lhs.mBPF->merge_or(*rhs.mBPF))
        {
            return lhs;
        }
    }


    Expression result;
    result.mType = Type::Or;
    result.or_append(std::move(lhs));
    result.or_append(std::move(rhs));

    std::shared_ptr<BPFCompositeExpression> expr;

    for (const Expression& child : result.mChildren)
    {
        if (!child.is_bpf())
        {
            return result;
        }

        if (!expr)
        {
            expr.reset(new BPFCompositeExpression(*child.mBPF));
            continue;
        }

        if (!expr->merge_or(*child.mBPF))
        {
            return result;
        }
    }

    result.mType = Type::BPF;
    result.mBPF = std::move(expr);
    result.mChildren.clear();
    return result;
}


void Expression::or_append(Expression&& expr)
{
    assert(is_or());

    if (expr.is_or())
    {
        auto b = std::make_move_iterator(expr.mChildren.begin());
        auto e = std::make_move_iterator(expr.mChildren.end());
        mChildren.insert(mChildren.end(), b, e);
    }
    else
    {
        mChildren.push_back(std::move(expr));
    }
}


Expression Expression::bpf_length(int value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(Length(value));
    return result;
}


Expression Expression::bpf_l3_type(EtherType ethertype)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(L3Type(ethertype));
    return result;
}


Expression Expression::bpf_l4_type(ProtocolId protocolId)

{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(L4Type(protocolId));
    return result;
}


Expression Expression::bpf_src_ip(IPv4Address value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(SourceIPv4(value));
    return result;
}


Expression Expression::bpf_dst_ip(IPv4Address value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(DestinationIPv4(value));
    return result;
}


Expression Expression::bpf_src_or_dst_ip(IPv4Address value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(SourceOrDestinationIPv4(value));
    return result;
}


Expression Expression::bpf_src_port(uint16_t value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(SourcePort(value));
    return result;
}


Expression Expression::bpf_dst_port(uint16_t value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(DestinationPort(value));
    return result;
}


Expression Expression::bpf_src_or_dst_port(uint16_t value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(SourceOrDestinationPort(value));
    return result;
}


Expression Expression::bpf_udp_payload(int offset, int size, uint32_t value)
{
    Expression result;
    result.mType = Type::BPF;
    result.mBPF = std::make_shared<BPFCompositeExpression>();
    result.mBPF->add_impl(UDPPayload(offset, size, value));
    result.print();
    return result;
}


bool Expression::match(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const
{
    switch (mType)
    {
        case Type::And:
        {
            if (mBPF && !mBPF->match(data, size, l3_offset, l4_offset))
            {
                return false;
            }
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
            assert(!mBPF);
            for (const Expression& child : mChildren)
            {
                if (child.match(data, size, l3_offset, l4_offset))
                {
                    return true;
                }
            }
            return false;
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
        case Type::BPF:
        {
            print_bpf(level);
            break;
        }
    }
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
