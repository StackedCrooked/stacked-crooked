#include <iostream>
#include <string>
#include <vector>


struct bpf_expression
{
    std::string toString() const
    {
        std::string result = protocol;

        if (!direction.empty())
        {
            result += (result.empty() ? "" : " ") + direction;
        }

        if (!type.empty())
        {
            result += (result.empty() ? "" : " ") + type;
        }

        if (!id.empty())
        {
            result += (result.empty() ? "" : " ") + id;
        }

        if (length > 0)
        {
            result += (result.empty() ? "" : " ") + ("len=" + std::to_string(length));
        }

        return result;
    }

    std::string protocol;
    std::string direction;
    std::string type;
    std::string id;
    int length = 0;
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

    Expression parse_bpf_expression()
    {
        bpf_expression expr;

        if (consume_token("ip"))
        {
            expr.protocol = "ip";
            if (consume_text("6"))
            {
                expr.protocol = "ip6";
            }

            if (consume_token("src"))
            {
                expr.direction = "src";

                std::string ip;
                if (!consume_ip4(ip))
                {
                    return error("IP");
                }

                expr.id = ip;
                return Expression::BPF(expr);
            }
            else if (consume_token("dst"))
            {
                expr.direction = "dst";

                if (!consume_ip4(expr.id))
                {
                    return error("IP");
                }

                return Expression::BPF(expr);
            }
            else
            {
                return Expression::BPF(expr);
            }
        }
        else
        {
            if (consume_token("udp"))
            {
                expr.protocol = "udp";
            }
            else if (consume_token("tcp"))
            {
                expr.protocol = "tcp";
            }
            else
            {
                return error("tcp or udp");
            }

            if (consume_token("src"))
            {
                expr.direction = "src";
                if (consume_token("port"))
                {
                    int port = 0;
                    if (consume_int(port))
                    {
                        expr.id = std::to_string(port);
                        return Expression::BPF(expr);
                    }
                    return error("integer");
                }

                return error("port");
            }
            else if (consume_token("dst"))
            {
                expr.direction = "dst";
                if (consume_token("port"))
                {
                    int port = 0;
                    if (consume_int(port))
                    {
                        expr.id = std::to_string(port);
                        return Expression::BPF(expr);
                    }
                    return error("integer");
                }
                return error("port");
            }
            else
            {
                return Expression::BPF(expr);
            }
        }
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

    bool consume_ip4(std::string& s)
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

        s = std::string(backup, mText);
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
    std::cout << prefix << str << std::endl;
    Parser p(str);
    try
    {
        Expression e = p.parse();
        //e.print();
        (void)e;
    }
    catch (const std::exception& e)
    {
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

    test("ip6");
    test("ip6 src 1.2.3.4");
    test("ip6 src 1.2.3.244 and udp");

    test("(ip)");
    test("(ip");
    test("ip)");
    test("(ip) and (udp)");
    test("(ip and udp) or (ip and tcp)");


    test("(ip and udp) or (ip and tcp) x)");  // JUNK NOT DETECTED
    test("(ip and udp) or (ip and tcp)  )");  // JUNK NOT DETECTED
    test("(ip and udp) or (ip and tcp) z)");  // JUNK NOT DETECTED


    test("(ip and ip src 1.2.3.44 and udp) or (ip6 and tcp)");
    test("ip and udp or ip6 and tcp");


    test("((((ip and udp) or (ip and tcp)) or (ip6 and udp or (ip6 and tcp))))");

    test("true or false");
    test("(udp and tcp) or false");

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
}

