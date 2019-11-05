#include "Parser.h"
#include <iomanip>
#include <iostream>
#include <sstream>


//static Expression test(const char* text)
//{
//    try
//    {
//        Parser p(text);
//        Expression result = p.parse();
//        //result.print();
//        return result;
//    }
//    catch (const std::exception& e)
//    {
//        std::cout << text << std::endl;
//        std::cout << e.what() << std::endl;
//        return Expression();
//    }
//}


//static Expression fTest = test("(ip and ip src 1.2.3.44) and (udp and udp src port 1024) and (len=120) and (udp dst port 1024)");
//static Expression fTest3 = test("(ip and ip src 1.2.3.44 and udp dst port 1024 and len=60) or (ip and ip src 1.2.3.44 and udp dst port 1024 and len=124) or (ip and ip src 1.2.3.44 and udp dst port 1024 and len=508) or (ip and ip src 1.2.3.44 and udp dst port 1024 and len=1020)");
//static Expression fTest2 = test("(ip and ip src 1.2.3.44 and udp dst port 1024 and len=60) or (ip and ip src 1.2.3.44 and udp dst port 1024 and len=124) or (ip and ip src 1.2.3.44 and udp dst port 1024 and len=508) or (ip and ip src 1.2.3.44 and udp dst port 1024 and len=1020)");


Parser::Parser(const char* text) :
    mOriginal(text),
    mText(text),
    mEnd(mOriginal + strlen(text))
{
}


Expression Parser::parse()
{
    if (consume_eof())
    {
        // Empty BPF returns "true" expression.
        return Expression::True();
    }

    auto result = parse_expression();

    if (!consume_eof())
    {
        return error("binary operator or end of expression");
    }

    //result.print();

    return result;
}

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
    else if (consume_text("udp["))
    {
        consume_whitespace();
        auto offset = 0;
        if (!consume_int(offset))
        {
            return error("byte offset");
        }

        if (!consume_text(":"))
        {
            return error(":");
        }

        auto width = 0;
        if (!consume_int(width))
        {
            return error("number of bytes");
        }

        if (width == 0)
        {
            return error("non-zero width value");
        }

        if (!consume_text("]"))
        {
            return error("]");
        }

        if (!consume_text("==") && !consume_text("="))
        {
            return error("=");
        }


        bool hex = consume_text("0x");



        uint32_t value = 0;
        std::istringstream iss(mText);
        if (!(iss >> (hex ? std::hex : std::dec) >> value))
        {
            return error("value");
        }

        std::cout << "VALUE=" << value << " to_string=" << std::to_string(value) << std::endl;

        while (is_alnum(*mText))
        {
            mText++;
        }

        return Expression::bpf_udp_payload(offset, width, value);
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

    throw std::runtime_error(std::string("Invalid filter: ") + mText);
}


bool Parser::consume_int(int& result)
{
    int n = 0;
    if (sscanf(mText, "%d%n", &result, &n) == 1)
    {
        mText += n;
        return true;
    }

    return false;
}


bool Parser::consume_uint8(uint8_t& result)
{
    int n = 0;
    int num_read = 0;
    if (sscanf(mText, "%d%n", &n, &num_read) == 1)
    {
        if (n >= 0 && n <= 255)
        {
            result = static_cast<uint8_t>(n);
            mText += num_read;
            return true;
        }
    }

    return false;
}


bool Parser::consume_ip4(IPv4Address& ip)
{
    consume_whitespace();

    auto backup = mText;

    if (consume_uint8(ip[0]) && consume_text_here(".") &&
        consume_uint8(ip[1]) && consume_text_here(".") &&
        consume_uint8(ip[2]) && consume_text_here(".") &&
        consume_uint8(ip[3]) && !is_alnum(*mText))
    {
        return true;
    }

    mText = backup;
    return false;
}

