#ifndef BPFFILTER_H
#define BPFFILTER_H


#include "Networking.h"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include "pcap.h"


struct BPFFilter
{
    static std::string get_bpffilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    BPFFilter(std::string bpf_filter);

    BPFFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
        BPFFilter(get_bpffilter(protocol, src_ip, dst_ip, src_port, dst_port))
    {
    }

    bool match(const uint8_t* data, uint32_t length) const
    {
        return bpf_filter_embedded(mProgram.bf_insns, const_cast<uint8_t*>(data), length, length);
    }

private:
    #define BPFFILTER_EXTRACT_SHORT(p)  ((u_short)ntohs(*(u_short *)p))
    #define BPFFILTER_EXTRACT_LONG(p) (ntohl(*(uint32_t *)p))

    static inline u_int bpf_filter_embedded(const struct bpf_insn* pc, const u_char* p, u_int wirelen, u_int buflen)
    {
        uint32_t A = 0;
        uint32_t X = 0;
        uint32_t k;
        uint32_t mem[BPF_MEMWORDS];

        if (pc == 0)
            /*
             * No filter means accept all.
             */
            return (u_int)-1;

        --pc;

        while (1)
        {
            ++pc;
            switch (pc->code)
            {

                default:
                    abort();
                case BPF_RET|BPF_K:
                    return (u_int)pc->k;

                case BPF_RET|BPF_A:
                    return (u_int)A;

                case BPF_LD|BPF_W|BPF_ABS:
                    k = pc->k;
                    if (k > buflen || sizeof(int32_t) > buflen - k)
                    {
                        return 0;
                    }
                    A = BPFFILTER_EXTRACT_LONG(&p[k]);
                    continue;

                case BPF_LD|BPF_H|BPF_ABS:
                    k = pc->k;
                    if (k > buflen || sizeof(int16_t) > buflen - k)
                    {
                        return 0;
                    }
                    A = BPFFILTER_EXTRACT_SHORT(&p[k]);
                    continue;

                case BPF_LD|BPF_B|BPF_ABS:
                {
                    k = pc->k;
                    if (k >= buflen)
                    {
                        return 0;
                    }
                    A = p[k];
                    continue;
                }
                case BPF_LD|BPF_W|BPF_LEN:
                    A = wirelen;
                    continue;

                case BPF_LDX|BPF_W|BPF_LEN:
                    X = wirelen;
                    continue;

                case BPF_LD|BPF_W|BPF_IND:
                    k = X + pc->k;
                    if (pc->k > buflen || X > buflen - pc->k ||
                            sizeof(int32_t) > buflen - k)
                    {
                        return 0;
                    }
                    A = BPFFILTER_EXTRACT_LONG(&p[k]);
                    continue;

                case BPF_LD|BPF_H|BPF_IND:
                    k = X + pc->k;
                    if (X > buflen || pc->k > buflen - X ||
                            sizeof(int16_t) > buflen - k)
                    {
                        return 0;
                    }
                    A = BPFFILTER_EXTRACT_SHORT(&p[k]);
                    continue;

                case BPF_LD|BPF_B|BPF_IND:
                    k = X + pc->k;
                    if (pc->k >= buflen || X >= buflen - pc->k)
                    {
                        return 0;
                    }
                    A = p[k];
                    continue;

                case BPF_LDX|BPF_MSH|BPF_B:
                    k = pc->k;
                    if (k >= buflen)
                    {
                        return 0;
                    }
                    X = (p[pc->k] & 0xf) << 2;
                    continue;

                case BPF_LD|BPF_IMM:
                    A = pc->k;
                    continue;

                case BPF_LDX|BPF_IMM:
                    X = pc->k;
                    continue;

                case BPF_LD|BPF_MEM:
                    A = mem[pc->k];
                    continue;

                case BPF_LDX|BPF_MEM:
                    X = mem[pc->k];
                    continue;

                case BPF_ST:
                    mem[pc->k] = A;
                    continue;

                case BPF_STX:
                    mem[pc->k] = X;
                    continue;

                case BPF_JMP|BPF_JA:
                    /*
                     * XXX - we currently implement "ip6 protochain"
                     * with backward jumps, so sign-extend pc->k.
                     */
                    pc += (bpf_int32)pc->k;
                    continue;

                case BPF_JMP|BPF_JGT|BPF_K:
                    pc += (A > pc->k) ? pc->jt : pc->jf;
                    continue;

                case BPF_JMP|BPF_JGE|BPF_K:
                    pc += (A >= pc->k) ? pc->jt : pc->jf;
                    continue;

                case BPF_JMP|BPF_JEQ|BPF_K:
                    pc += (A == pc->k) ? pc->jt : pc->jf;
                    continue;

                case BPF_JMP|BPF_JSET|BPF_K:
                    pc += (A & pc->k) ? pc->jt : pc->jf;
                    continue;

                case BPF_JMP|BPF_JGT|BPF_X:
                    pc += (A > X) ? pc->jt : pc->jf;
                    continue;

                case BPF_JMP|BPF_JGE|BPF_X:
                    pc += (A >= X) ? pc->jt : pc->jf;
                    continue;

                case BPF_JMP|BPF_JEQ|BPF_X:
                    pc += (A == X) ? pc->jt : pc->jf;
                    continue;

                case BPF_JMP|BPF_JSET|BPF_X:
                    pc += (A & X) ? pc->jt : pc->jf;
                    continue;

                case BPF_ALU|BPF_ADD|BPF_X:
                    A += X;
                    continue;

                case BPF_ALU|BPF_SUB|BPF_X:
                    A -= X;
                    continue;

                case BPF_ALU|BPF_MUL|BPF_X:
                    A *= X;
                    continue;

                case BPF_ALU|BPF_DIV|BPF_X:
                    if (X == 0)
                        return 0;
                    A /= X;
                    continue;

                //        case BPF_ALU|BPF_MOD|BPF_X:
                //            if (X == 0)
                //                return 0;
                //            A %= X;
                //            continue;

                case BPF_ALU|BPF_AND|BPF_X:
                    A &= X;
                    continue;

                case BPF_ALU|BPF_OR|BPF_X:
                    A |= X;
                    continue;

                //        case BPF_ALU|BPF_XOR|BPF_X:
                //            A ^= X;
                //            continue;

                case BPF_ALU|BPF_LSH|BPF_X:
                    A <<= X;
                    continue;

                case BPF_ALU|BPF_RSH|BPF_X:
                    A >>= X;
                    continue;

                case BPF_ALU|BPF_ADD|BPF_K:
                    A += pc->k;
                    continue;

                case BPF_ALU|BPF_SUB|BPF_K:
                    A -= pc->k;
                    continue;

                case BPF_ALU|BPF_MUL|BPF_K:
                    A *= pc->k;
                    continue;

                case BPF_ALU|BPF_DIV|BPF_K:
                    A /= pc->k;
                    continue;

                //        case BPF_ALU|BPF_MOD|BPF_K:
                //            A %= pc->k;
                //            continue;

                case BPF_ALU|BPF_AND|BPF_K:
                    A &= pc->k;
                    continue;

                case BPF_ALU|BPF_OR|BPF_K:
                    A |= pc->k;
                    continue;

                //        case BPF_ALU|BPF_XOR|BPF_K:
                //            A ^= pc->k;
                //            continue;

                case BPF_ALU|BPF_LSH|BPF_K:
                    A <<= pc->k;
                    continue;

                case BPF_ALU|BPF_RSH|BPF_K:
                    A >>= pc->k;
                    continue;

                case BPF_ALU|BPF_NEG:
                    /*
                     * Most BPF arithmetic is unsigned, but negation
                     * can't be unsigned; throw some casts to
                     * specify what we're trying to do.
                     */
                    A = (uint32_t)(-(int32_t)A);
                    continue;

                case BPF_MISC|BPF_TAX:
                    X = A;
                    continue;

                case BPF_MISC|BPF_TXA:
                    A = X;
                    continue;
            }
        }
    }

    bpf_program mProgram;
};


#endif // BPFFILTER_H
