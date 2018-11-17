#include "Networking.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>


enum Flags : uint32_t
{
    Flags_none     = 0,
    Flags_ip       = 1 << 0,
    Flags_ip_src   = 1 << 1,
    Flags_ip_dst   = 1 << 2,
    Flags_udp      = 1 << 3,
    Flags_udp_src  = 1 << 4,
    Flags_udp_dst  = 1 << 5,
    Flags_length   = 1 << 6
};


Flags operator|(Flags lhs, Flags rhs)
{
    return Flags(uint32_t(lhs) | uint32_t(rhs));
}


Flags& operator|=(Flags& lhs, Flags rhs)
{
    return lhs = lhs | rhs;
}


Flags operator&(Flags lhs, Flags rhs)
{
    return Flags(uint32_t(lhs) & uint32_t(rhs));
}


Flags& operator&=(Flags& lhs, Flags rhs)
{
    return lhs = lhs & rhs;
}


struct PacketInfo
{
    uint32_t layer3_offset = 0;
    uint32_t layer4_offset = 0;
    Flags flags = Flags_none;
};


struct RxPacket
{
    RxPacket()
    {
        mIPv4Header.src_ip = IPv4Address(1, 1, 1, 1); // FOR NOW
        mIPv4Header.dst_ip = IPv4Address(1, 1, 1, 1); // FOR NOW
        mUDPHeader.src_port = 1024;
        mUDPHeader.dst_port = 1024;
    }
    const uint8_t* data() const { return mData; }
    uint32_t size() const { return mSize; }

    const uint8_t* mData = nullptr;
    uint16_t mSize = 0;
    Flags mFlags = Flags_none;

    IPv4Header mIPv4Header;  // FOR NOW
    UDPHeader mUDPHeader;    // FOR NOW

};


struct bpf_expression
{
    std::string toString() const
    {
        return std::to_string(mFlags);
    }

    void add_flag(Flags flag)
    {
        mFlags |= flag;
    }

    bool match_any(Flags flag)
    {
        return mFlags & flag;
    }

    bool match_one(Flags flag)
    {
        return (mFlags & flag) == flag;
    }

    bool evaluate(const RxPacket& rx_packet, const PacketInfo& /*info*/) const
    {
        //auto data = rx_packet.data();
        //auto l3 = info.layer3_offset;
        //auto l4 = info.layer4_offset;

        auto ip_header = rx_packet.mIPv4Header;//Decode<IPv4Header>(data + l3);
        auto udp_header = rx_packet.mUDPHeader; //Decode<UDPHeader>(data + l4);
        if (mFlags & Flags_ip_src)
        {
            if (src_ip != ip_header.src_ip)
            {
                return false;
            }
        }
        if (mFlags & Flags_ip_dst)
        {
            if (dst_ip != ip_header.dst_ip)
            {
                return false;
            }
        }
        if (mFlags & Flags_udp_src)
        {
            if (src_port != udp_header.src_port.hostValue())
            {
                return false;
            }
        }
        if (mFlags & Flags_udp_dst)
        {
            if (dst_port != udp_header.dst_port.hostValue())
            {
                return false;
            }
        }
        if (mFlags & Flags_length)
        {
            if (length != rx_packet.size())
            {
                return false;
            }
        }

        return true;
    }

    Flags mFlags = Flags_none;
    IPv4Address src_ip;
    IPv4Address dst_ip;
    int src_port = 0;
    int dst_port = 0;
    uint32_t length = 0;
};


struct Expression
{
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

    static Expression Boolean(bool b)
    {
        Expression result;
        result.mType = Type::Bool;
        result.mValue = static_cast<int>(b);
        return result;
    }

    static Expression Length(int value)
    {
        Expression result;
        result.mType = Type::Length;
        result.mValue = value;
        return result;
    }

    static Expression BPF(bpf_expression expr)
    {
        Expression result;
        result.mType = Type::BPF;
        result.mBPF = expr;
        return result;
    }

    enum class Type
    {
        And, Or, BPF, Bool, Length
    };

    void print(int level = 0) const
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
            case Type::Bool:
            {
                print_bool(level);
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

    void print_bool(int level) const
    {
        std::cout << indent(level) << "<bpf>" << (mValue ? "true" : "false") << "</bpf>\n";
    }

    void print_length(int level) const
    {
        std::cout << indent(level) << "<bpf>len=" << mValue << "</bpf>\n";
    }

    void print_bpf(int level) const
    {
        std::cout << indent(level) << "<bpf>" << mBPF.toString() << "</bpf>\n";
    }

    void print_binary(const char* op, int level) const
    {
        std::cout << indent(level) << "<" << op << ">\n";
        for (const Expression& child : mChildren)
        {
            child.print(level + 2);
        }
        std::cout << indent(level) << "</" << op << ">\n";
    }

    static std::string indent(int level)
    {
        std::string result;
        for (auto i = 0; i != level; ++i)
        {
            result.push_back(' ');
        }
        return result;
    }

    bool evaluate(const RxPacket& rx_packet, const PacketInfo& info) const
    {
        if (mType == Type::BPF)
        {
            return mBPF.evaluate(rx_packet, info);
        }
        if (mType == Type::And)
        {
            for (const Expression& c : mChildren)
            {
                if (!c.evaluate(rx_packet, info))
                {
                    return false;
                }
            }
            return true;
        }
        else if (mType == Type::Or)
        {
            for (const Expression& c : mChildren)
            {
                if (c.evaluate(rx_packet, info))
                {
                    return true;
                }
            }
            return false;
        }
        else if (mType == Type::Length)
        {
            return mValue == static_cast<int32_t>(rx_packet.size());
        }
        else if (mType == Type::Bool)
        {
            return mValue;
        }

        return true;
    }

    Type mType = Type::And;
    int mValue = 0;
    bpf_expression mBPF;
    std::vector<Expression> mChildren;
};


struct Parser
{
    explicit Parser(const char* text) :
        mOriginal(text),
        mText(text)
    {
    }

    Expression parse()
    {
        auto result = parse_logical_expression();

        if (!consume_eof())
        {
            return error("eof");
        }

        return result;
    }

private:
    Expression parse_logical_expression()
    {
        auto result = parse_unary_expression();

        if (consume_token("and"))
        {
            return Expression::And(result, parse_logical_expression());
        }
        else if (consume_token("or"))
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
        if (consume_token("("))
        {
            auto result = parse_logical_expression();

            if (!consume_token(")"))
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
        if (consume_token("true"))
        {
            return Expression::Boolean(true);
        }
        else if (consume_token("false"))
        {
            return Expression::Boolean(false);
        }
        else if (consume_token("len"))
        {
            if (!consume_token("==") && !consume_token("="))
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

    void consume_flags(bpf_expression& expr)
    {
        if (consume_token("ip src"))
        {
            if (consume_ip4(expr.src_ip))
            {
                expr.add_flag(Flags_ip_src);
            }
        }
        else if (consume_token("ip dst"))
        {
            if (consume_ip4(expr.dst_ip))
            {
                expr.add_flag(Flags_ip_dst);
            }
        }
        else if (consume_token("ip"))
        {
            expr.add_flag(Flags_ip);
        }
        else if (consume_token("udp src port"))
        {
            int n = 0;
            if (consume_int(n))
            {
                expr.add_flag(Flags_udp_src);
                expr.src_port = n;
            }
        }
        else if (consume_token("udp dst port"))
        {
            int n = 0;
            if (consume_int(n))
            {
                expr.add_flag(Flags_udp_dst);
                expr.dst_port = n;
            }
        }
        else if (consume_token("udp"))
        {
            expr.add_flag(Flags_udp);
        }
    }

    Expression parse_bpf_expression()
    {
        bpf_expression expr;

        consume_flags(expr);

        return Expression::BPF(expr);
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

    bool consume_text(const char* token)
    {
        return consume_text_impl(token, strlen(token));
    }

    bool consume_text_impl(const char* token, int len)
    {
        if (!strncmp(mText, token, len))
        {
            mText += len;
            return true;
        }

        return false;
    }

    bool check_text(const char* token)
    {
        return check_text_impl(token, strlen(token));
    }

    bool check_text_impl(const char* token, int len)
    {
        return !strncmp(mText, token, len);
    }

    bool consume_token(const char* token)
    {
        consume_whitespace();
        return consume_text(token);
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
        return consume_ip4(ip.data());
    }

    bool consume_ip4(uint8_t* bytes)
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


        bytes[0] = a;
        bytes[1] = b;
        bytes[2] = c;
        bytes[3] = d;
        return true;
    }

    Expression error(std::string expected = "")
    {
        throw std::runtime_error(std::string(mText - mOriginal, ' ') + "^--- Expected: " + expected);
    }

    const char* const mOriginal;
    const char* mText;
};


void test_(const char* file, int line, const char* str)
{
    std::string prefix = std::string(file) + ":" + std::to_string(line) + ": ";
    Parser p(str);
    try
    {
        Expression e = p.parse();
        //e.print();
        (void)e;
    }
    catch (const std::exception& e)
    {
        std::cout << prefix << str << std::endl;
        std::cerr << std::string(prefix.size(), ' ') << e.what() << std::endl;
        
    }
}


#define test(s) test_(__FILE__, __LINE__, s)


#define ASSERT_TRUE(expr) if (!(expr)) { std::cerr << __FILE__ << ":" << __LINE__ << ": Assertion failure: ASSERT_TRUE(" << #expr << ")" << std::endl; }
#define ASSERT_EQ(x, y) if (x != y) { std::cerr << __FILE__ << ":" << __LINE__ << ": Assertion failure: ASSERT_EQ(" << #x << "(" << x << "), " << #y << "(" << y << "))\n"; }
int main()
{
    test("ip");
    test("ip src 1.2.3.4");
    test("ip src 1.2.3.244 and udp");
    test("ip src 1.2.3.244 and rpg");
    test("ip src 1.2.3.24o and rpg");

    test("(ip)");
    test("(ip");
    test("ip)");
    test("(ip) and (udp)");


    test("true or false");

    test("ip");
    test("ip and ip src 192.168.12.13");
    test("ip and ip src 255.255.255.255");
    test("udp src port 1024");
    test("ip and ip src 1.1.1.1 and ip dst 1.1.1.2 and udp and udp src port 1024 and udp dst port 1024");
    test("ip and ip src 1.1.1.1 and ip dst 1.1.1.2 and udp and udp src port 1024 and udp dst port 1024 and len=128");
    test("len==12 and true");
    test("len ==  12 and true");
    test("len ==12 and true");
    test("len== 12 and true");


    test("len=12 and true");
    test("len =  12 and true");
    test("len =12 and true");
    test("len= 12 and true");

    test("len==12 and true");
    test("len==12 and false");

    test("len==12a");
    test("len==12 a");
    test("len===12a"); // SHOULD FAIL
    test("len===12a"); // SHOULD FAIL


    //
    // PARSER + EVALUATOR
    //

    Parser p = Parser("ip src 1.1.1.1 and (ip dst 1.1.1.3 or ip dst 1.1.1.2)");
    Expression e = p.parse();


    PacketInfo info = PacketInfo();
    info.flags |= Flags_ip;
    info.flags |= Flags_ip_src;
    info.flags |= Flags_ip_dst;
    info.flags |= Flags_udp;
    info.flags |= Flags_udp_src;
    info.flags |= Flags_udp_dst;
    RxPacket rx_packet = RxPacket();

    rx_packet.mIPv4Header.src_ip = IPv4Address(1, 1, 1, 1);
    rx_packet.mIPv4Header.dst_ip = IPv4Address(1, 1, 1, 2);

    std::cout << "Expecte match: " << e.evaluate(rx_packet, info) << std::endl;

    rx_packet.mIPv4Header.dst_ip = IPv4Address(1, 1, 1, 3);

    std::cout << "Expect match: " << e.evaluate(rx_packet, info) << std::endl;

    rx_packet.mIPv4Header.dst_ip = IPv4Address(1, 1, 1, 4);

    std::cout << "Expect no match: " << e.evaluate(rx_packet, info) << std::endl;
}

