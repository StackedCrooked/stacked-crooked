#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>


#define VerifyWithFileAndWithLine(file, line, statement) \
    if (true == (statement)) \
    { \
        std::cout << file << ":" << line << ": " << #statement << ": OK\n"; \
    } \
    else \
    { \
        std::cout << file << ":" << line << ": " << #statement << ": *** FAIL ***\n"; \
    }


#define Verify(statement) VerifyWithFileAndWithLine(__FILE__, __LINE__, statement);


template <class T>
class Node
{
public:
    typedef T value_type;

    Node() { }

    Node(const value_type & value) : _value(value) { }

    typedef Node<value_type>                        this_type;
    typedef typename std::vector<this_type>         container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    const_iterator begin() const
    {
        return _children.begin();
    }

    const_iterator end() const
    {
        return _children.end();
    }

    iterator begin()
    {
        return _children.begin();
    }

    iterator end()
    {
        return _children.end();
    }

    void insert(const this_type & item)
    {
        _children.push_back(item);
    }

    size_t size() const
    {
        return _children.size();
    }

    bool empty() const
    {
        return _children.empty();
    }

    value_type & get()
    {
        return _value;
    }

    const value_type & get() const
    {
        return _value;
    }

    void set(const value_type & value)
    {
        _value = value;
    }

private:
    value_type _value;
    container_type _children;
};


template<class T>
typename Node<T>::const_iterator FindCycle(const Node<T> & inNode,
                                           std::vector<const T*> inPreceding = std::vector<const T*>())
{
    typename Node<T>::const_iterator it = inNode.begin(), end = inNode.end();
    for (; it != end; ++it)
    {
        const Node<T> & child = *it;
        if (std::find(inPreceding.begin(), inPreceding.end(), &child.get()) != inPreceding.end())
        {
            return it;
        }
        else
        {
            inPreceding.push_back(&inNode.get());

            // Previous nodes object is passed by value!
            // This is an important aspect of the algorithm!
            typename Node<T>::const_iterator childIt = FindCycle(child, inPreceding);
            if (childIt != child.end())
            {
                return childIt;
            }
        }
    }
    return it;
}


template<class T>
bool HasCycles(const Node<T> & node)
{
    return FindCycle(node) != node.end();
}


struct Mutex
{
    Mutex(char inChar) :
        mChar(inChar)
    {

    }

    static Node<Mutex*> & Get()
    {
        static Node<Mutex*> fList;
        return fList;
    }

    void lock()
    {
        Get().insert(this);
    }

    void unlock()
    {
        // No action required.
    }

    char mChar;
};


void Lock(Mutex & a)
{
    a.lock();
}


void Lock(Mutex & a,
          Mutex & b)
{
    a.lock();
    b.lock();
}


void Lock(Mutex & a,
          Mutex & b,
          Mutex & c)
{
    a.lock();
    b.lock();
    c.lock();
}


void Lock(Mutex & a,
          Mutex & b,
          Mutex & c,
          Mutex & d)
{
    a.lock();
    b.lock();
    c.lock();
}


void testNode()
{
    // Defined the tree with PointerPolicy_Normal_NoOwnership to void deleting stack-allocated objects.

    Mutex a('a');
    Mutex b('b');
    Mutex c('c');
    Mutex d('d');

    Lock(a, b);
    Verify(!HasCycles(Mutex::Get()));

    Lock(a, c);
    Verify(!HasCycles(Mutex::Get()));

    Lock(b, d);
    Verify(!HasCycles(Mutex::Get()));

    Lock(a, b, c);
    Verify(!HasCycles(Mutex::Get()));

    Lock(a, b, d);
    Verify(!HasCycles(Mutex::Get()));

    Lock(a, b, c, d);
    Verify(!HasCycles(Mutex::Get()));

    Lock(a, b, d, c);
    Verify(!HasCycles(Mutex::Get()));

    Lock(b, a);
    Verify(HasCycles(Mutex::Get()));
}


int main()
{
    testNode();
    return 0;
}
