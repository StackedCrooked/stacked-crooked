#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <vector>


/**
 * (ip and ip src 95.174.242.22 and ip dst 212.60.47.101 and udp src port 55555 and udp dst port 55555 and len=60  ) or
 * (ip and ip src 95.174.242.22 and ip dst 212.60.47.101 and udp src port 55555 and udp dst port 55555 and len=508 ) or
 * (ip and ip src 95.174.242.22 and ip dst 212.60.47.101 and udp src port 55555 and udp dst port 55555 and len=1514)
 */




static std::string indent(int level)
{
    std::string result;
    for (auto i = 0; i != level; ++i)
    {
        result.push_back(' ');
    }
    return result;
}


static bool validate_ipv4_address(const std::string& s)
{
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    if (sscanf(s.c_str(), "%d%*[.]%d%*[.]%d%*[.]%d", &a, &b, &c, &d) != 4)
    {
        return false;
    }

    auto check = [](int n) { return n >= 0 && n <= 255; };

    return check(a) && check(b) && check(c) && check(d);
}


enum SubType
{
    none,
    ip,
    ip_src,
    ip_dst,
    ip6,
    ip6_src,
    ip6_dst,
    udp,
    udp_src_port,
    udp_dst_port,
    tcp,
    tcp_src_port,
    tcp_dst_port,
    len
};


std::string subtype_names[] = {
    "none",
    "ip",
    "ip_src",
    "ip_dst",
    "ip6",
    "ip6_src",
    "ip6_dst",
    "udp",
    "udp_src_port",
    "udp_dst_port",
    "tcp",
    "tcp_src_port",
    "tcp_dst_port",
    "len"
};


std::ostream& operator<<(std::ostream& os, SubType st)
{
    return os << subtype_names[st];
}


struct CheckSourceIP
{
    explicit CheckSourceIP(uint32_t value) : mIPAddress(value)
    {

    }

    bool check_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l3_size*/, uint32_t /*l4_offset*/, uint32_t /*l4_size*/) const
    {
        return *reinterpret_cast<const uint32_t*>(data + l3_offset) == mIPAddress;
    }

    uint32_t mIPAddress;
};


struct AbstractExpressionImpl
{
    virtual bool check(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l3_size, uint32_t l4_offset, uint32_t l4_size) const = 0;
};



template<typename T>
struct ConcreteExpressionImpl final : AbstractExpressionImpl
{
    explicit ConcreteExpressionImpl(T t) : t(t) {}

    bool check(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l3_size, uint32_t l4_offset, uint32_t l4_size) const override
    {
        return t.check_impl(data, size, l3_offset, l3_size, l4_offset, l4_size);
    }

    T t;
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

    static Expression Leaf(SubType subtype)
    {
        Expression result;
        result.mType = Type::Leaf;
        result.mSubType = subtype;
        return result;
    }

    static Expression Leaf(SubType subtype, int a)
    {
        Expression result;
        result.mType = Type::Leaf;
        result.mSubType = subtype;
        result.mArg1 = a;
        return result;
    }

    static Expression Leaf(SubType subtype, int a, int b)
    {
        Expression result;
        result.mType = Type::Leaf;
        result.mSubType = subtype;
        result.mArg1 = a;
        result.mArg2 = b;
        return result;
    }

    static Expression Leaf(SubType subtype, const std::string& value)
    {
        Expression result;
        result.mType = Type::Leaf;
        result.mSubType = subtype;
        result.mValue = value;
        return result;
    }

    template<typename T>
    static Expression MakeLeaf(T t)
    {
        Expression result;
        result.mImpl = std::make_shared<ConcreteExpressionImpl<T>>(t);
        result.mType = Type::Leaf;
        return result;
    }

    enum class Type
    {
        And,
        Or,
        Leaf
    };

    void print(int level = 0)
    {
        switch (mType)
        {
            case Type::And:
            {
                print_binary("And", level);
                break;
            }
            case Type::Or:
            {
                print_binary("Or", level);
                break;
            }
            case Type::Leaf:
            {
                print_leaf(level);
                break;
            }
        }
    }

    void print_leaf(int level)
    {
        std::cout << indent(level) << "Leaf: " << mSubType;
        if (mArg1) { std::cout << " " << mArg1; }
        if (mArg2) { std::cout << " " << mArg2; }
        if (!mValue.empty()) { std::cout << " " << mValue; }
        std::cout << "\n";
    }

    void print_binary(const char* op, int level)
    {
        std::cout << indent(level) << op << ":" << std::endl;
        for (Expression& e : mChildren)
        {
            e.print(level + 1);
        }
    }

    std::shared_ptr<const AbstractExpressionImpl> mImpl;
    Type mType = Type::Leaf;
    SubType mSubType = none;
    uint16_t mArg1 = 0;
    uint16_t mArg2 = 0;
    std::string mValue;
    std::vector<Expression> mChildren;
};


#define MUST_CONSUME_INT() must_consume_int(__FILE__, __LINE__)
#define MUST_CONSUME(s) must_consume(s, __FILE__, __LINE__)
#define MUST_CONSUME_IP() must_consume_ip(__FILE__, __LINE__)

struct Parser
{
    explicit Parser(const std::string& text) :
        mString(text)
    {
    }

    Expression parse_expression()
    {
        auto leaf = parse_group_expression();

        if (consume("and"))
        {
            return Expression::And(leaf, parse_expression());
        }
        else if (consume("or"))
        {
            return Expression::Or(leaf, parse_expression());
        }
        else
        {
            auto backup = mData;
            auto s = consume();
            if (!s.empty())
            {
                mData = backup;
                error(__FILE__, __LINE__, "end of expression or a logical operator like \"and\" or \"or\"");
            }

            return leaf;
        }
    }

    Expression parse_group_expression()
    {
        if (consume("("))
        {
            auto result = parse_expression();
            MUST_CONSUME(")");
            return result;
        }
        else
        {
            return parse_leaf_expression();
        }
    }

    Expression parse_leaf_expression()
    {
        if (consume("a"))
        {
            throw std::runtime_error("a is bad");
        }
        if (consume("ip6"))
        {
            if (consume("src"))
            {
                return Expression::Leaf(ip6_src, consume());
            }
            else if (consume("dst"))
            {
                return Expression::Leaf(ip6_dst, consume());
            }
            else
            {
                return Expression::Leaf(ip6);
            }
        }
        else if (consume("ip"))
        {
            if (consume("src"))
            {
                return Expression::Leaf(ip_src, MUST_CONSUME_IP());
            }
            else if (consume("dst"))
            {
                return Expression::Leaf(ip_dst, MUST_CONSUME_IP());
            }
            else
            {
                return Expression::Leaf(ip);
            }
        }
        else if (consume("udp"))
        {
            if (consume("src"))
            {
                MUST_CONSUME("port");
                return Expression::Leaf(udp_src_port, MUST_CONSUME_INT());
            }
            else if (consume("dst"))
            {
                MUST_CONSUME("port");
                return Expression::Leaf(udp_dst_port, MUST_CONSUME_INT());
            }
            else
            {
                return Expression::Leaf(udp);
            }
        }
        else if (consume("tcp"))
        {
            if (consume("src"))
            {
                MUST_CONSUME("port");
                return Expression::Leaf(tcp_src_port, MUST_CONSUME_INT());
            }
            else if (consume("dst"))
            {
                MUST_CONSUME("port");
                return Expression::Leaf(tcp_dst_port, MUST_CONSUME_INT());
            }
            else
            {
                return Expression::Leaf(udp);
            }
        }
        else if (consume("len"))
        {
            if (consume("=="))
            {
                return Expression::Leaf(len, MUST_CONSUME_INT());
            }

            MUST_CONSUME("=");
            return Expression::Leaf(len, MUST_CONSUME_INT());
        }

        return error(__FILE__, __LINE__, "BPF Expression");
    }

    bool consume(const std::string& s)
    {
        skip_whitespace();

        if (!strncmp(mData, s.c_str(), s.size()))
        {
            mData += s.size();

            return true;
        }

        return false;
    }

    void must_consume(const std::string& s, const char* file, int line)
    {
        skip_whitespace();
        auto backup = mData;
        if (!consume(s))
        {
            mData = backup;
            error(file, line, "\"" + s + "\"");
        }
    }


    int must_consume_int(const char* file, int line)
    {
        skip_whitespace();
        auto int_string = consume_int_string();

        auto backup = mData;
        int n = 0;
        if (!boost::conversion::try_lexical_convert(int_string, n))
        {
            mData = backup;
            error(file, line, "Integer[" + int_string + "]");
            throw std::runtime_error("BPF Parsing failed");
        }
        return n;
    }


    int must_consume_ip(const char* file, int line)
    {
        skip_whitespace();
        auto backup = mData;
        auto ip_string = consume();
        if (!validate_ipv4_address(ip_string))
        {
            mData = backup;
            error(file, line, "valid ip string");
        }

        return 0;
    }

    std::string consume()
    {
        skip_whitespace();


        std::string result;

        for (;;)
        {
            auto c = *mData;

            if (c == 0 || is_whitespace(c) || c == ')')
            {
                break;
            }

            result.push_back(c);
            mData++;
        }

        return result;
    }

    std::string consume_int_string()
    {
        skip_whitespace();

        std::string result;

        for (;;)
        {
            auto c = *mData;

            if (c < '0' || c > '9')
            {
                break;
            }

            result.push_back(c);
            mData++;
        }

        return result;
    }

    static bool is_whitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\n';
    }

    void skip_whitespace()
    {
        for (;;)
        {
            char c = *mData;
            if (c == 0)
            {
                break;
            }

            if (!is_whitespace(c))
            {
                break;
            }
            mData++;
        }
    }

    Expression error(const char* file, int line, const std::string& expectation);

    std::string mString;
    const char* mData = mString.c_str();
    uint32_t mIndex = 0;
    uint32_t mSize = mString.size();
};


void test(const char* str)
{
    std::cout << "=== TEST: " << str << " ===" << std::endl;
    try
    {
        Parser p(str);
        Expression e = p.parse_expression();
        e.print();
    }
    catch (const std::exception&)
    {
        //
    }
    std::cout << std::endl;
}


int main()
{
    test("ip and udp dst port 1024 and udp src port 1024 and len == 12 a");
    test("ip and udp dst port 1024 b and udp src port 1024 and len == 1");
    test("ip and udp dst a port 1024 b and udp src port 1024 and len == 1");
    test("ip");
    test("ip6");
    test("ip or ip6");
    test("(ip)");
    test("(ip6)");
    test("(ip) or (ip6)");
    test("((ip) or (ip6))");

    test("len=128");
    test("ip and udp dst port 1024 and udp src port 1024 and len =128");
    test("len= 128");
    test("len = 128");

    test("len==128");
    test("len ==128");
    test("len== 128");
    test("len == 128");

    test("ip and udp dst port 1024 and udp src port 1024 and len==12a");
    test("ip and udp dst port 1024 and udp src port 1024 and len== 12a");
    test("ip and udp dst port 1024 and udp src port 1024 and len == 12 a");
    test("ip and udp dst port 1024 and udp src port 1024 and len == b128");

    test("udp");
    test("udp src port 1024");
    test("udp dst port 1024");

    test("tcp");
    test("tcp src port 1024");
    test("tcp dst port 1024");

    test("ip and udp src port 1024");
    test("ip and udp dst port 1024");
    test("ip and udp dst port 1024 and udp src port 1024");

    test("ip src 1.2.3.244 and udp");

    try { test("ip src 1.2.3 and udp"); } catch (...) {}


    test("(ip and ip src 1.2.3.44 and udp) or (ip6 and tcp)");
    test("ip and udp or ip6 and tcp");  // => TODO: AND should have precedence over OR

    test("((ip and udp) or (ip6 and tcp)) and ((ip and udp src port 1024) or (ip6 and tcp))");

    test("((((ip and udp) or (ip and tcp)) or (ip6 and udp or (ip6 and tcp))))");
}


Expression Parser::error(const char *file, int line, const std::string& expectation)
{
    std::cerr << '\n' << file << ":" << line << ": Parser error" << std::endl;
    std::cerr << "\t" << mString << "\n\t" << std::string(mData - mString.data(), ' ')  << "^ Expected " << expectation << std::endl;
    throw std::runtime_error("Invalid BPF expression");
}
