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

    Node() : _parent(NULL) { }

    Node(Node<T> * parent, const value_type & value) :
        _parent(parent),
        _value(value)
    {
    }

    typedef Node<value_type>                        this_type;
    typedef typename std::vector<this_type>         container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    Node<T> & parent()
    {
        return * _parent;
    }

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

    Node<T> & insert(const value_type & item)
    {
        _children.push_back(Node<T>(this, item));
        return _children.back();
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
    Node<T> * _parent;
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

#if 0

 m1 m2 // OK
 m2 m1 // Cycle!

 m1 m2 // OK
 m1 m3
 m1 m3 m2 // OK
 m1 m2 m3 // Cycle!

#endif
struct Mutex
{
     static Node<Mutex*> & GetRoot()
     {
         static Node<Mutex*> root;
         return root;
     }

     static Node<Mutex*> & GetLast()
     {
         static Node<Mutex*> last = GetRoot();
         return last;
     }

    Mutex(char inChar) :
        mChar(inChar),
        mPrevious()
    {
    }

    void lock()
    {
        GetLast() = GetLast().insert(this);
    }

    void unlock()
    {
        GetLast() = GetLast().parent();
    }

    char mChar;
    Mutex * mPrevious;
};


struct ScopedLock
{
    ScopedLock(Mutex & inMutex) :
        mMutex(inMutex)
    {
        mMutex.lock();
    }

    ~ScopedLock()
    {
        mMutex.unlock();
    }

    Mutex mMutex;
};



void testNode()
{
    Mutex a('a');
    Mutex b('b');

    {
        ScopedLock lock_a(a);
        ScopedLock lock_b(b);
        Verify(!HasCycles(Mutex::GetRoot()));
    }

    {
        ScopedLock lock_a(b);
        ScopedLock lock_b(a);
        Verify(HasCycles(Mutex::GetRoot()));
    }
}


int main()
{
    testNode();
    return 0;
}
