#pragma once


#include "Expression.h"
#include "Networking.h"
#include <cstring>
#include <string>


struct Parser
{
    explicit Parser(const char* text) :
        mOriginal(text),
        mText(text),
        mEnd(mOriginal + strlen(text))
    {
    }

    Expression parse()
    {
        if (consume_eof())
        {
            // Empty BPF returns "true" expression.
            return Expression::True();
        }

        auto result = parse_logical_expression();

        if (!consume_eof())
        {
            return error("binary operator or end of expression");
        }

        return result;
    }

    Expression parse_logical_expression()
    {
        auto result = parse_unary_expression();

        if (consume_token("and") || consume_text("&&"))
        {
            return Expression::And(result, parse_logical_expression());
        }
        else if (consume_token("or") || consume_text("||"))
        {
            return Expression::Or(result, parse_logical_expression());
        }
        else
        {
            return result;
        }
    }

    Expression parse_unary_expression()
    {
        if (consume_text("("))
        {
            auto result = parse_logical_expression();

            if (!consume_text(")"))
            {
                return error("')'");
            }

            return result;
        }
        else
        {
            return parse_attribute();
        }
    }

    Expression parse_attribute()
    {
        if (consume_token("len"))
        {
            if (!consume_text("==") && !consume_text("="))
            {
                return error("equal sign ('=')");
            }

            int len = 0;
            if (!consume_int(len))
            {
                return error("digit");
            }
            return Expression::Length(len);
        }
        else
        {
            return parse_bpf_expression();
        }
    }

    Expression parse_bpf_expression()
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

    bool consume_eof()
    {
        consume_whitespace();
        return is_eof();
    }

    bool is_eof() const
    {
        return *mText == '\0';
    }

    void consume_whitespace()
    {
        while (is_space(*mText))
        {
            ++mText;
        }
    }

    bool consume_token(const char* token)
    {
        consume_whitespace();

        auto backup = mText;

        if (!consume_text(token))
        {
            return false;
        }

        if (is_alnum(*mText))
        {
            mText = backup;
            return false;
        }

        return true;
    }

    bool consume_text(const char* token)
    {
        consume_whitespace();
        return consume_text_impl(token, strlen(token));
    }

    bool consume_text_impl(const char* token, int len)
    {
        if (len > static_cast<int>(mEnd - mText))
        {
            return false;
        }

        if (!strncmp(mText, token, len))
        {
            mText += len;
            return true;
        }

        return false;
    }

    bool consume_int(int& n) // TODO: FR: consider overflow
    {
        consume_whitespace();

        if (!is_digit(*mText))
        {
            return false;
        }

        for (;;)
        {
            n = (10 * n) + (*mText++ - '0');

            if (!is_digit(*mText))
            {
                break;
            }
        }

        return true;
    }

    bool is_alnum(char c) const
    {
        return is_digit(c) || is_lcase(c) || is_ucase(c);
    }

    bool is_alpha(char c) const
    {
        return is_lcase(c) || is_ucase(c);
    }

    bool is_delim(char c) const
    {
        return is_space(c) || is_eof() || c == ')';
    }

    bool is_lcase(char c) const
    {
        return c >= 'a' && c <= 'z';
    }

    bool is_ucase(char c) const
    {
        return c >= 'A' && c <= 'Z';
    }

    bool is_digit(char c) const
    {
        return c >= '0' && c <= '9';
    }

    bool is_space(char c) const
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    bool consume_ip4(IPv4Address& ip)
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

    Expression error(std::string expected = "")
    {
        throw std::runtime_error(std::string(mText - mOriginal, ' ') + "^--- Expected: " + expected);
    }

    const char* const mOriginal;
    const char* mText;
    const char* mEnd;
};
