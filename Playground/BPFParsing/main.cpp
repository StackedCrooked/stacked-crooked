#include <boost/tokenizer.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <vector>


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


struct Parser
{
    explicit Parser(const std::string& text) :
        mTokens(tokenize(text))
    {
    }

    Expression parse_bpf_expression()
    {
        if (next_token("ip") || next_token("ip6") || next_token("udp") || next_token("tcp") || next_token("ether") || next_token("ppp"))
        {
            auto leaf = parse_leaf_expression();

            return parse_binary_expression(leaf);
        }
        else if (consume_token("("))
        {
            auto result = parse_bpf_expression();

            if (consume_token(")"))
            {
                return parse_binary_expression(result);
            }
            else
            {
                return error(__FILE__, __LINE__);
            }
        }
        else
        {
            return error(__FILE__, __LINE__);
        }
    }

    Expression parse_binary_expression(Expression result)
    {
        if (consume_token("and"))
        {
            return Expression::And(result, parse_bpf_expression());
        }
        else if (consume_token("or"))
        {
            return Expression::Or(result, parse_bpf_expression());
        }
        else
        {
            return result;
        }
    }

    Expression parse_leaf_expression()
    {
        return Expression::Leaf(pop_token());
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

    bool next_token(const std::string& s) const
    {
        return !mTokens.empty() && mTokens.front() == s;
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
    test("ip and udp");


    test("(ip and udp) or (ip6 and tcp)");
    test("ip and udp or ip6 and tcp");  // => TODO: AND should have precedence over OR

    test("((ip and udp) or (ip6 and tcp)) and (ether or ppp)");

    test("((((ip and udp) or (ip and tcp)) or (ip6 and udp or (ip6 and tcp))))");
}
