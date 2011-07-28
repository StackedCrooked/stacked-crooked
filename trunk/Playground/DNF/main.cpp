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
        int count = 0;
        while (!result->isDNF())
        {
            result = result->getDNFImpl();
            if (count++ == 20)
            {
                throw;
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

    Type leftType() const { return left()->getType(); }

    Type rightType() const { return right()->getType(); }

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
        if ((getType() != leftType()) && (leftType() != Type_Unary))
        {
            leftString = group(leftString);
        }

        std::string rightString = right()->toString();

        // A and (B or C)

        // Reduce brackets so that we see (A and B and C) instead of ((A and B) and C)
        if ((getType() != rightType()) && (rightType() != Type_Unary))
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
        return Or(And(leftLeaf, rightAnd->left()->getDNF()),
                  And(leftLeaf, rightAnd->right()->getDNF()));
    }

    // (A and B) or C => (A and C) or (B and C)
    BinaryExpression * leftAnd = dynamic_cast<AndExpression*>(left());
    if (leftAnd && rightLeaf)
    {
        return Or(And(leftAnd->left()->getDNF(),  rightLeaf),
                  And(leftAnd->right()->getDNF(), rightLeaf));
    }

    return Or(left()->getDNF(), right()->getDNF());
}


bool AndExpression::isDNFImpl() const
{
    if (leftType() == Type_Or || rightType() == Type_Or)
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

    OrExpression * leftOr = dynamic_cast<OrExpression*>(left());
    OrExpression * rightOr = dynamic_cast<OrExpression*>(right());

    // (A or B) and (C or D) => (A and C) or (A and D) or (B and C) or (B and D)
    if (leftOr && rightOr)
    {
        ExpressionPtr a = leftOr->left()->getDNF();
        ExpressionPtr b = leftOr->right()->getDNF();
        ExpressionPtr c = rightOr->left()->getDNF();
        ExpressionPtr d = rightOr->right()->getDNF();
        return Or(Or(And(a, c),
                     And(a, d)),
                  Or(And(b, c),
                     And(b, d)));
    }

    // (A or B) and C => (A and C) or (B and C)
    if (leftOr && rightLeaf)
    {
        return Or(And(leftOr->left()->getDNF(),  rightLeaf),
                  And(leftOr->right()->getDNF(), rightLeaf));
    }

    // A and (B or C) -> (A and B) or (A and C)
    if (leftLeaf && rightOr)
    {
        return Or(And(leftLeaf, rightOr->left()->getDNF()),
                  And(leftLeaf, rightOr->right()->getDNF()));
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


int main()
{

    // A and (B or C)
    test(And(A, Or(B, C)));

    // (A or B) and C
    test(And(Or(A, B), C));

    // (A or B) and (C or D)
    test(And(Or(A, B),
             Or(C, D)));

    // (A or B) and (C or D) and (E or F)
    test(And(Or(A, B),
             Or(C, D),
             Or(E, F)));

    // (A and B) or (A and C)
    test(Or(And(A, B),
            And(C, D)));

    // (A and B) or (A and C)
    test(Or(And(A, B),
            And(C, D),
            And(E, F)));

    // (A and B) or (A and C)
    test(And(Or(A, B, C),
             Or(D, E, F)));

    // TODO: Fix FAIL!!
    // (A and B) and (E or F))
//    test(And(And(A, B),
//             Or(E, F)));

    return 0;
}
