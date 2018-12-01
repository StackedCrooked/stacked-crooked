#ifndef EXPRESSION_H
#define EXPRESSION_H


#include "BPFExpression.h"
#include "Networking.h"
#include "Utils.h"
#include <memory>
#include <string>
#include <vector>



struct BPFCompositeExpression;



struct Expression
{
    static Expression True();
    static Expression And(Expression lhs, Expression rhs);
    static Expression Or(Expression lhs, Expression rhs);
    static Expression bpf_length(int value);
    static Expression bpf_l3_type(EtherType ethertype);
    static Expression bpf_l4_type(ProtocolId protocolId);
    static Expression bpf_src_ip(IPv4Address value);
    static Expression bpf_dst_ip(IPv4Address value);
    static Expression bpf_src_or_dst_ip(IPv4Address value);
    static Expression bpf_src_port(uint16_t value);
    static Expression bpf_dst_port(uint16_t value);
    static Expression bpf_src_or_dst_port(uint16_t value);
    static Expression bpf_udp_payload(int offset, int size, uint32_t value);

    bool match(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const;

    enum class Type
    {
        And, Or, BPF
    };

    void print(int level = 0) const;

    void print_bpf(int level) const;
    void print_binary(const char* op, int level) const;

    static std::string indent(int level);

    void and_append(Expression expr);
    void or_append(Expression&& expr);

    bool is_and() const { return mType == Type::And; }
    bool is_or() const { return mType == Type::Or; }
    bool is_bpf() const { return mType == Type::BPF; }

    Type mType = Type::And;
    std::shared_ptr<BPFCompositeExpression> mBPF;
    std::vector<Expression> mChildren;
};


#endif // EXPRESSION_H
