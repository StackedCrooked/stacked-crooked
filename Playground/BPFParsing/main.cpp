#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/tokenizer.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <vector>


template<typename T>
using Optional = boost::optional<T>;


typedef boost::tokenizer<boost::char_separator<char>> tokenizer;


std::deque<std::string> tokenize(const std::string& str)
{
    std::deque<std::string> result;

    boost::char_separator<char> sep(" ", "()=");

    tokenizer tokens(str, sep);
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        result.push_back(*tok_iter);
    }

    return result;
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

    static Expression Boolean(bpf_expression expr)
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

    Type mType = Type::And;
    int mValue = 0;
    bpf_expression mBPF;
    std::vector<Expression> mChildren;
};


struct Parser
{
    explicit Parser(const std::string& text) :
        mTokens(tokenize(text))
    {
    }

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

            if (consume_token(")"))
            {
                return result;
            }
            else
            {
                return error(__FILE__, __LINE__);
            }
        }
        else
        {
            return parse_bpf_expression();
        }
    }

    Expression parse_bpf_expression()
    {
        if (consume_token("true"))
        {
            return Expression::Boolean(true);
        }

        if (consume_token("false"))
        {
            return Expression::Boolean(false);
        }

        if (consume_token("len"))
        {
            if (!consume_token("="))
            {
                return error(__FILE__, __LINE__, "'=' sign");
            }

            if (consume_token("="))
            {
                // ignore second '='
            }

            auto length_string = pop_token(__FILE__, __LINE__, "\"len\" expects a length value");
            int length = 0;
            if (boost::conversion::try_lexical_convert(length_string,  length))
            {
                return Expression::Length(length);
            }

            return error(__FILE__, __LINE__, "Length value should be an integer.");
        }


        bpf_expression expr;

        expr.protocol = consume_one_of({"ether", "ppp", "arp", "ip", "ip6", "udp", "tcp"});
        expr.direction = consume_one_of({"src", "dst"});
        expr.type = consume_one_of({"host", "port"});

        if (!expr.direction.empty() || !expr.type.empty())
        {
            auto token = pop_token(__FILE__, __LINE__, "Expected value for " + expr.protocol + " " + expr.direction + " " + expr.type);

            if (!is_number(token) && !validate_ipv4_address(token))
            {
                return error(__FILE__, __LINE__, expr.type == "port" ? "Port number" : "IP address");
            }

            expr.id = token;
        }

        return Expression::Boolean(expr);
    }

    bool consume_token(const std::string& s)
    {
        if (mTokens.empty() || mTokens.front() != s)
        {
            return false;
        }

        pop_token();
        return true;
    }

    std::string consume_one_of(std::vector<std::string> tokens, std::string default_result = "")
    {
        for (const std::string& token : tokens)
        {
            if (consume_token(token))
            {
                return token;
            }
        }

        return default_result;
    }

    const std::string& peak_token()
    {
        if (mTokens.empty())
        {
            error(__FILE__, __LINE__);
            throw 1;
        }

        return mTokens.front();
    }

    std::string pop_token()
    {
        assert(!mTokens.empty());
        auto result = std::move(mTokens.front());
        mTokens.pop_front();
        return result;
    }

    std::string pop_token(const char* file, int line, const std::string& expected)
    {
        if (mTokens.empty())
        {
            error(file, line, expected);
            return "";
        }

        return pop_token();
    }

    bool is_number(const std::string& s)
    {
        auto p = [](char c) { return c < 0 || c > 9; };
        auto b = s.begin();
        auto e = s.end();
        return std::find_if(b, e, p) != e;
    }

    Expression error(const char* file, int line)
    {
        std::cerr << file << ":" << line << ": " << (mTokens.empty() ? std::string("Expected more tokens") : ("Unexpected token: " + mTokens.front())) << std::endl;
        exit(1);
        throw 1;
    }

    Expression error(const char* file, int line, std::string message)
    {
        std::cerr << file << ":" << line << ": " << message << std::endl;
        exit(1);
        throw 1;
    }

    std::deque<std::string> mTokens;
};


void test(const char* str)
{
    std::cout << "=== TEST: " << str << " ===" << std::endl;
    Parser p(str);
    Expression e = p.parse_logical_expression();
    e.print();
    std::cout << std::endl;
}


int main()
{
    test("ip");
    test("ip src 1.2.3.244 and udp");


    test("(ip and ip src 1.2.3.44 and udp) or (ip6 and tcp)");
    test("ip and udp or ip6 and tcp");  // => TODO: AND should have precedence over OR


    test("((((ip and udp) or (ip and tcp)) or (ip6 and udp or (ip6 and tcp))))");

    test("true or false");
    test("(udp and tcp) or false");
    test("((ip and udp) or (ip6 and tcp)) and (ether or ppp)");

    test("ip");
    test("ip and ip src 1.1.1.1");
    test("udp src port 1024");
    test("ip and ip src 1.1.1.1 and ip dst 1.1.1.2 and udp and udp src port 1024 and udp dst port 1024");
    test("ip and ip src 1.1.1.1 and ip dst 1.1.1.2 and udp and udp src port 1024 and udp dst port 1024 and len=128");
    test("len===12 and true");
}
