#include <stdexcept>
#include <string>
#include <iostream>


class BinaryExpression;
class UnaryExpression;


class Expression
{
public:
    typedef Expression * ExpressionPtr;

    virtual ~Expression() {}

    ExpressionPtr getDNF() const
    {
        // Multiple passes are sometimes required.
        ExpressionPtr result = getDNFImpl();
        int safety = 0;
        while (!result->isDNF())
        {
            std::cout << "Pass " << ": " << result->toString() << std::endl;
            result = result->getDNFImpl();
            if (safety++ > 100)
            {
                throw std::runtime_error("Maximum number of DNF passes exceeded.");
            }
        }
        return result;
    }

    bool isDNF() const
    {
        return isDNFImpl();
    }

    std::string toString()
    {
        return toStringImpl();
    }

    enum Type {
        Type_Unary,
        Type_And,
        Type_Or
    };

    Type getType() const
    {
        return mType;
    }

    Expression * clone() const
    {
        return cloneImpl();
    }

protected:
    Expression(Type inType) : mType(inType)
    {
    }

private:
    virtual ExpressionPtr getDNFImpl() const= 0;

    virtual bool isDNFImpl() const = 0;

    virtual std::string toStringImpl() = 0;

    virtual ExpressionPtr cloneImpl() const = 0;

    Type mType;
};

typedef Expression::ExpressionPtr ExpressionPtr;


class UnaryExpression : public Expression
{
public:
    static ExpressionPtr New(const std::string & inValue)
    {
        ExpressionPtr result(new UnaryExpression(inValue));
        return result;
    }

    virtual ~UnaryExpression() {}

    const std::string & value() const
    {
        return mValue;
    }

protected:
    UnaryExpression(const std::string & inValue) :
        Expression(Type_Unary),
        mValue(inValue)
    {
    }

private:
    virtual bool isDNFImpl() const
    {
        return true;
    }

    virtual ExpressionPtr getDNFImpl() const
    {
        return clone();
    }

    virtual std::string toStringImpl()
    {
        return mValue;
    }

    virtual ExpressionPtr cloneImpl() const
    {
        return UnaryExpression::New(mValue);
    }

    std::string mValue;
};


class BinaryExpression : public Expression
{
public:

    virtual ~BinaryExpression() { }

    const ExpressionPtr left() const
    {
        return mLeft;
    }

    ExpressionPtr left()
    {
        return mLeft;
    }

    const ExpressionPtr right() const
    {
        return mRight;
    }

    ExpressionPtr right()
    {
        return mRight;
    }

    std::string getKeyWord() const
    {
        if (getType() == Type_And)
        {
            return "and";
        }
        else if (getType() == Type_Or)
        {
            return "or";
        }
        throw std::logic_error("Invalid enum value.");
    }

protected:
    BinaryExpression(Type inType, ExpressionPtr inLeft, ExpressionPtr inRight) :
        Expression(inType),
        mLeft(inLeft),
        mRight(inRight)
    {
    }

    static std::string group(const std::string & text)
    {
        return "(" + text + ")";
    }

private:

    virtual std::string toStringImpl()
    {
        std::string leftString = left()->toString();

        // Reduce brackets so that we see (A and B and C) instead of ((A and B) and C)
        if ((getType() != left()->getType()) && (left()->getType() != Type_Unary))
        {
            leftString = group(leftString);
        }

        std::string rightString = right()->toString();

        // A and (B or C)

        // Reduce brackets so that we see (A and B and C) instead of ((A and B) and C)
        if ((getType() != right()->getType()) && (right()->getType() != Type_Unary))
        {
            rightString = group(rightString);
        }

        return leftString + " " + getKeyWord() + " " + rightString;
    }

    ExpressionPtr mLeft;
    ExpressionPtr mRight;
};


class OrExpression : public BinaryExpression
{
public:
    static ExpressionPtr New(ExpressionPtr inLeft, ExpressionPtr inRight)
    {
        ExpressionPtr result(new OrExpression(inLeft, inRight));
        return result;
    }

    virtual ~OrExpression() {}

private:

    OrExpression(ExpressionPtr inLeft, ExpressionPtr inRight) :
        BinaryExpression(Type_Or, inLeft, inRight)
    {
    }

    virtual ExpressionPtr cloneImpl() const
    {
        return OrExpression::New(left()->clone(), right()->clone());
    }

    virtual bool isDNFImpl() const;

    virtual ExpressionPtr getDNFImpl() const;
};


class AndExpression : public BinaryExpression
{
public:
    static ExpressionPtr New(ExpressionPtr inLeft, ExpressionPtr inRight)
    {
        ExpressionPtr result(new AndExpression(inLeft, inRight));
        return result;
    }

    virtual ~AndExpression() {}

private:
    AndExpression(ExpressionPtr inLeft, ExpressionPtr inRight) :
        BinaryExpression(Type_And, inLeft, inRight)
    {
    }

    virtual ExpressionPtr cloneImpl() const
    {
        return AndExpression::New(left()->clone(), right()->clone());
    }

    virtual bool isDNFImpl() const;

    virtual ExpressionPtr getDNFImpl() const;
};


ExpressionPtr And(ExpressionPtr left, ExpressionPtr right)
{
    return AndExpression::New(left, right);
}


ExpressionPtr And(ExpressionPtr arg0, ExpressionPtr arg1, ExpressionPtr arg2)
{
    return And(And(arg0, arg1), arg2);
}


ExpressionPtr Or(ExpressionPtr left, ExpressionPtr right)
{
    return OrExpression::New(left, right);
}


ExpressionPtr Or(ExpressionPtr arg0, ExpressionPtr arg1, ExpressionPtr arg2)
{
    return Or(Or(arg0, arg1), arg2);
}


bool OrExpression::isDNFImpl() const
{
    return left()->isDNF() && right()->isDNF();
}


ExpressionPtr OrExpression::getDNFImpl() const
{
    UnaryExpression * leftLeaf = dynamic_cast<UnaryExpression*>(left());
    UnaryExpression * rightLeaf = dynamic_cast<UnaryExpression*>(right());

    // A or B
    if (leftLeaf && rightLeaf)
    {
        return clone();
    }

    // A or (B and C) => (A and B) or (A and C)
    BinaryExpression * rightAnd = dynamic_cast<AndExpression*>(right());
    if (leftLeaf && rightAnd)
    {
        return Or(And(leftLeaf, rightAnd->left()),
                  And(leftLeaf, rightAnd->right()));
    }

    // (A and B) or C => (A and C) or (B and C)
    BinaryExpression * leftAnd = dynamic_cast<AndExpression*>(left());
    if (leftAnd && rightLeaf)
    {
        return Or(And(leftAnd->left(),  rightLeaf),
                  And(leftAnd->right(), rightLeaf));
    }

    return Or(left()->getDNF(), right()->getDNF());
}


bool AndExpression::isDNFImpl() const
{
    if (left()->getType() == Type_Or || right()->getType() == Type_Or)
    {
        return false;
    }
    return left()->isDNF() && right()->isDNF();
}


ExpressionPtr AndExpression::getDNFImpl() const
{
    UnaryExpression * leftLeaf = dynamic_cast<UnaryExpression*>(left());
    UnaryExpression * rightLeaf = dynamic_cast<UnaryExpression*>(right());

    if (leftLeaf && rightLeaf)
    {
        return clone();
    }

    // (A or B) and C => (A and C) or (B and C)
    if (OrExpression * leftOr = dynamic_cast<OrExpression*>(left()))
    {
        return Or(And(leftOr->left(),  right()),
                  And(leftOr->right(), right()));
    }

    // A and (B or C) -> (A and B) or (A and C)
    if (OrExpression * rightOr = dynamic_cast<OrExpression*>(right()))
    {
        return Or(And(left(), rightOr->left()),
                  And(left(), rightOr->right()));
    }

    return And(left()->getDNF(), right()->getDNF());
}

#define A UnaryExpression::New("A")
#define B UnaryExpression::New("B")
#define C UnaryExpression::New("C")
#define D UnaryExpression::New("D")
#define E UnaryExpression::New("E")
#define F UnaryExpression::New("F")
#define G UnaryExpression::New("G")
#define H UnaryExpression::New("H")


void test(ExpressionPtr expr)
{
    std::cout << "Before DNF: " << expr->toString() << std::endl;
    std::cout << "After DNF : " << expr->getDNF()->toString() << std::endl << std::endl;
}


void run()
{

    test(And(A, Or(B, C)));

    test(And(Or(A, B), C));

    test(And(Or(A, B),
             Or(C, D)));

    test(And(Or(A, B, C),
             Or(D, E, F)));

    test(And(Or(A, B),
             Or(C, D),
             Or(E, F)));

    test(Or(And(A, B),
            And(C, D)));

    test(Or(And(A, B, C),
             And(D, E, F)));

    test(Or(And(A, B),
            And(C, D),
            And(E, F)));


    test(And(And(Or(A, B),
                 And(C, D)),
             And(E, F)));

    test(And(And(A, B),
             Or(E, F)));

    test(And(And(A, B),
             Or(C, D, And(E, F))));

    test(And(Or(A, B),
             And(C, D),
             Or(E, F)));

    test(And(A, Or(B, And(C, Or(D, And(E, F))))));
}


int main()
{
    try
    {
        run();
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
    }
    return 0;
}
