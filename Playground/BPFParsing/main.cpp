#include <boost/tokenizer.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <vector>


typedef boost::tokenizer<boost::char_separator<char>> tokenizer;


std::deque<std::string> tokenize(const std::string& str)
{
    //std::string str = "ip src 10.1.2.3 and (ip dst 10.4.5.6) and udp src port 1000 and udp dst port 2000 and len = 12 and len=13 udp[1:2] = 1";

    std::deque<std::string> result;

    boost::char_separator<char> sep(" ", "()=");

    tokenizer tokens(str, sep);
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        result.push_back(*tok_iter);
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

    void print_leaf(int level)
    {
        std::cout << indent(level) << "Leaf: " << mValue << std::endl;
    }

    void print_binary(const char* key, int level)
    {
        std::cout << indent(level) << key << ":" << std::endl;
        for (Expression& e : mChildren)
        {
            e.print(level + 1);
        }
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

    Type mType = Type::Leaf;
    std::string mValue;
    std::vector<Expression> mChildren;
};


struct Parser
{
    explicit Parser(const std::string& text) :
        mTokens(tokenize(text))
    {
//        std::cout << "=== Parser ===" << std::endl;
//        std::cout << "Tokens:" << std::endl;
//        for (const std::string& s : mTokens)
//        {
//            std::cout << "  [" << s << "]" << std::endl;
//        }
//        std::cout << std::endl;
//        std::cout << std::endl;
    }

    Expression parse_bpf_expression()
    {
        if (next_token("ip") || next_token("ip6") || next_token("udp") || next_token("tcp"))
        {
            auto leaf = parse_leaf_expression();

            if (consume_token("and"))
            {
                return Expression::And(leaf, parse_bpf_expression());
            }
            else if (consume_token("or"))
            {
                return Expression::Or(leaf, parse_bpf_expression());
            }
            else
            {
                return leaf;
            }
        }
        else if (consume_token("("))
        {
            auto result = parse_bpf_expression();
            if (consume_token("and"))
            {
                return Expression::And(result, parse_bpf_expression());
            }
            else if (consume_token("or"))
            {
                return Expression::Or(result, parse_bpf_expression());
            }
            else if (consume_token(")"))
            {
                return result;
            }

            std::cout << __FILE__ << ":" << __LINE__ << " LEAF OK" << std::endl;
            return error("BAD TOKEN");
        }
        else
        {

            std::cout << __FILE__ << ":" << __LINE__ << " LEAF OK" << std::endl;
            return error("BAD TOKEN");
        }
    }

    Expression parse_or_expression()
    {
        Expression result = parse_and_expression();

        while (consume_token("or"))
        {
            result = Expression::Or(result, parse_and_expression());
        }

        return result;
    }

    Expression parse_and_expression()
    {
        Expression result = parse_bpf_expression();

        while (consume_token("and"))
        {
            result = Expression::And(result, parse_bpf_expression());
        }

        return result;
    }

    bool consume_token(const std::string& s)
    {
        if (mTokens.empty())
        {
            return false;
        }

        if (mTokens.front() != s)
        {
            return false;
        }

        pop();
        return true;
    }

    bool next_token(const std::string& s) const
    {
        //std::cout << "CHECK: [" << s << "] tokens.front=" << mTokens.front() << " " << (s == mTokens.front()) << std::endl;
        return !mTokens.empty() && mTokens.front() == s;
    }

    Expression parse_leaf_expression()
    {
        return Expression::Leaf(pop());
    }

    std::string pop()
    {
        auto result = mTokens.front();
        mTokens.pop_front();

        mText += result;
        //std::cout << "POP: [" << result << "]" << std::endl;
        return result;
    }

    Expression error(const std::string& s)
    {
        std::cout << mText << std::endl;
        std::cout << Expression::indent(mText.size()) << "^" << std::endl;
        std::cout << s << std::endl;
        exit(1);
        throw 1;
    }


    std::string mText;
    std::deque<std::string> mTokens;
    uint32_t mAdvanced = 0;
};


void test(const char* str)
{
    std::cout << "=== TEST: " << str << " ===" << std::endl;
    Parser p(str);
    Expression e = p.parse_or_expression();
    e.print();
    std::cout << std::endl;
}


int main()
{

    test("ip");
    test("(ip)");
    test("((ip))");

    test("(ip and udp)");

    test("(ip) and (udp)");
    test("((ip) and (udp))");

    test("(ip and udp)");
    test("((ip and udp))");
    test("((ip and udp))");

    test("(ip and udp) or (ip and tcp)");

    test("((ip and udp) or (ip and tcp)) and ((ip6 and udp) or (ip6 and tcp))");
}
