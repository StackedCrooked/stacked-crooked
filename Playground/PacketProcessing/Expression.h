#ifndef EXPRESSION_H
#define EXPRESSION_H


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

    void print_bool(int level) const;
    void print_length(int level) const;
    void print_bpf(int level) const;
    void print_binary(const char* op, int level) const;

    static std::string indent(int level);

    Type mType = Type::And;
    int mValue = 0;
    bpf_expression mBPF;
    std::vector<Expression> mChildren;
};



#endif // EXPRESSION_H
