#include <boost/tokenizer.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include <set>


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

    static Expression Or()
    {
        Expression result;
        result.mType = Type::Or;
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

    static Expression Leaf(const std::string& s)
    {
        Expression result;
        result.mType = Type::Leaf;
        result.mValue = s;
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
        std::cout << indent(level) << "Leaf: " << mValue << std::endl;
    }

    void print_binary(const char* op, int level)
    {
        std::cout << indent(level) << op << ":" << std::endl;
        for (Expression& e : mChildren)
        {
            e.print(level + 1);
        }
    }

    Type mType = Type::Leaf;
    std::string mValue;
    std::vector<Expression> mChildren;
};


/**
 * simplified from https://github.com/the-tcpdump-group/libpcap/blob/master/grammar.y
 * depend on the functionality of data plane
 * BPF: http://biot.com/capstats/bpf.html
 *
 *
 *  letter = "A-Za-z"
 *  digit = "0-9"
 *
 *
 *  node_name = letter { letter | digit | "_" }
 *  node_type = letter { letter }
 *
 *
 *  node = node_name "::" node_type
 *  edge = node_name [ "[" lexpr "]" ] "->" [ "[" lexpr "]" ] node_name
 *
 *
 *  attribute = "true" | "false" | bpf_expr | nf_expr
 *  bpf_expr = bpf_term bpf_id
 *
 *
 *  bpf_id = bpf_ipv4 | digit {digit}
 *  bpf_ipv4 = bpf_ipv4_octet "." bpf_ipv4_octet "." bpf_ipv4_octet "." bpf_ipv4_octet
 *  bpf_ipv4_octet = "0-9" | "1-9" "0-9" | "1" "0-9" "0-9" | "2" "0-4" "0-9" | "2" "5" "0-5"
 *
 *
 *  bpf_term = [bpf_proto] [bpf_dir] [bpf_type] bpf_id
 *  bpf_proto = "ether" | "fddi" | "tr" | "wlan" | "ip" | "ip6" | "arp" | "rarp" | "decnet" | "tcp" | "udp"
 *  bpf_dir = "src" | "dst" | "src or dst" | "src and dst"
 *  bpf_type = "host" | "net" | "port" | "portrange"
 *
 *
 *  nf_expr = nf_type nf_attr
 *  // nf_type = load nf types from user specification
 *  // nf_attr = load nf-specific attributes from user specification
 *
 *
 *  lexpr =
 *    "(" lexpr ")"     |
 *    lexpr "and" lexpr |
 *    lexpr "or" lexpr  |
 *    attribute
 *
 */
struct Parser
{
    explicit Parser(const std::string& text) :
        mTokens(tokenize(text))
    {
        mProtocols = {
            "ether", "ppp", "ip", "ip6", "udp", "tcp"
        };
    }

    Expression parse_bpf_expression()
    {
        if (consume_token("("))
        {
            Expression result = parse_bpf_expression();

            if (!consume_token(")"))
            {
                return error(__FILE__, __LINE__);
            }

            return result;
        }
        else
        {
            return parse_or_expression();
        }
    }

    Expression parse_or_expression()
    {
        auto result = parse_and_expression();

        if (consume_token("or"))
        {
            result = Expression::Or(result, parse_bpf_expression());
        }

        return result;
    }

    Expression parse_and_expression()
    {
        if (is_attribute())
        {
            auto attribute = Expression::Leaf(pop_token());

            if (!consume_token("and"))
            {
                return attribute;
            }

            auto result = Expression::And(attribute, parse_bpf_expression());

            while (consume_token("and"))
            {
                result.mChildren.push_back(parse_bpf_expression());
            }

            return result;
        }
        else
        {
            return error(__FILE__, __LINE__);
        }
    }

    bool is_attribute() const
    {
        return !mTokens.empty() && mProtocols.count(mTokens.front());
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

    std::string pop_token()
    {
        assert(!mTokens.empty());
        auto result = mTokens.front();
        mTokens.pop_front();
        return result;
    }

    Expression error(const char* file, int line)
    {
        std::cout << file << ":" << line << ": " <<
            (mTokens.empty() ? std::string("Expected more tokens") : ("Unexpected token: " + mTokens.front())) << std::endl;
        exit(1);
        throw 1;
    }


    std::deque<std::string> mTokens;
    std::set<std::string> mProtocols;
};


void test(const char* str)
{
    std::cout << "=== TEST: " << str << " ===" << std::endl;
    Parser p(str);
    Expression e = p.parse_bpf_expression();
    e.print();
    std::cout << std::endl;
}


int main()
{
    test("ip");
    test("(ip)");

    test("ip and udp");
    test("(ip and udp)");

    test("ether and ip and udp");
    test("ether and ppp and ip and udp");

    test("ip or udp");
    test("(ip or udp)");

    test("(ip and udp) or (ip6 and tcp)");
    test("ip and udp or ip6 and tcp");
}
