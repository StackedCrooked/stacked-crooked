#include "Parser.h"


Expression Parser::parse_bpf_expression()
{
    if (consume_token("ip"))
    {
        if (consume_token("src"))
        {
            IPv4Address ip;
            if (consume_ip4(ip))
            {
                return Expression::bpf_src_ip(ip);
            }
            return error("Valid IPv4 address");
        }
        else if (consume_token("dst"))
        {
            IPv4Address ip;
            if (consume_ip4(ip))
            {
                return Expression::bpf_dst_ip(ip);
            }
            return error("Valid IPv4 address");
        }
        else if (consume_token("host"))
        {
            IPv4Address ip;
            if (consume_ip4(ip))
            {
                return Expression::bpf_src_or_dst_ip(ip);
            }
            return error("Valid IPv4 address");
        }
        else
        {
            return Expression::bpf_l3_type(EtherType::IPv4);
        }
    }
    else if (consume_token("ip6"))
    {
        throw std::runtime_error("TODO: IP6");
    }
    else if (consume_token("udp"))
    {
        if (consume_token("src port"))
        {
            int n = 0;
            if (consume_int(n))
            {
                return Expression::bpf_src_port(n);
            }
            return error("port number");
        }
        else if (consume_token("dst port"))
        {
            int n = 0;
            if (consume_int(n))
            {
                return Expression::bpf_dst_port(n);
            }
            return error("port number");
        }
        else if (consume_token("port"))
        {
            int n = 0;
            if (consume_int(n))
            {
                return Expression::bpf_src_or_dst_port(n);
            }
            return error("port number");
        }
        else
        {
            return Expression::bpf_l4_type(ProtocolId::UDP);
        }
    }
    else if (consume_token("tcp"))
    {
        if (consume_token("src port"))
        {
            int n = 0;
            if (consume_int(n))
            {
                return Expression::bpf_src_port(n);
            }
            return error("port number");
        }
        else if (consume_token("dst port"))
        {
            int n = 0;
            if (consume_int(n))
            {
                return Expression::bpf_dst_port(n);
            }
            return error("port number");
        }
        else if (consume_token("port"))
        {
            int n = 0;
            if (consume_int(n))
            {
                return Expression::bpf_src_or_dst_port(n);
            }
            return error("port number");
        }
        else
        {
            return Expression::bpf_l4_type(ProtocolId::TCP);
        }
    }
    else
    {
        consume_whitespace();
        if (consume_text("udp["))
        {
            throw std::runtime_error("TODO: UDP payload");
        }
    }

    throw std::runtime_error("Invalid filter");
}


bool Parser::consume_ip4(IPv4Address& ip)
{
    consume_whitespace();

    auto backup = mText;

    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    if (!consume_int(a))
    {
        mText = backup;
        return false;
    }
    if (!consume_text("."))
    {
        mText = backup;
        return false;
    }
    if (!consume_int(b))
    {
        mText = backup;
        return false;
    }
    if (!consume_text("."))
    {
        mText = backup;
        return false;
    }
    if (!consume_int(c))
    {
        mText = backup;
        return false;
    }
    if (!consume_text("."))
    {
        mText = backup;
        return false;
    }
    if (!consume_int(d))
    {
        mText = backup;
        return false;
    }

    auto check = [](int n) { return n >= 0 && n <= 255; };

    if (!check(a) || !check(b) || !check(c) || !check(d))
    {
        mText = backup;
        return false;
    }

    // IP should be followed by delim
    auto end_of_ip = mText;
    if (is_alnum(*mText))
    {
        mText = end_of_ip;
        return false;
    }

    ip[0] = a;
    ip[1] = b;
    ip[2] = c;
    ip[3] = d;

    return true;
}

