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
    typedef this_type*                              ptr_type;
    typedef typename std::vector<ptr_type>          container_type;
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

    void insert(this_type * inItem)
    {
        ptr_type item(inItem);
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
typename Node<T>::const_iterator FindCycle(const Node<T> & node,
                                           std::vector<const T*> preceding = std::vector<const T*>())
{
    typename Node<T>::const_iterator it = node.begin(), end = node.end();
    for (; it != end; ++it)
    {
        Node<T> & child = **it;
        if (std::find(preceding.begin(), preceding.end(), &child.get()) != preceding.end())
        {
            return it;
        }
        else
        {
            preceding.push_back(&node.get());

            // Previous nodes object is passed by value!
            // This is an important aspect of the algorithm!
            typename Node<T>::const_iterator childIt = FindCycle(child, preceding);
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
    Mutex(char inChar) : mChar(inChar) {}

    void lock() {}
    void unlock() {}

    char mChar;
};


template<class T>
void Lock(T & a, T & b)
{
    a.insert(&b);
}


template<class T>
void Lock(T & a, T & b, T& c)
{
    a.insert(&b);
    b.insert(&c);
}


template<class T>
void Lock(T & a, T & b, T& c, T & d)
{
    a.insert(&b);
    b.insert(&c);
    c.insert(&d);
}


void testNode()
{
    // Defined the tree with PointerPolicy_Normal_NoOwnership to void deleting stack-allocated objects.
    typedef Node<Mutex> MutexNode;
    MutexNode a('a');
    MutexNode b('b');
    MutexNode c('c');
    MutexNode d('d');

    Lock(a, b);
    Verify(!HasCycles(a));
    Verify(!HasCycles(b));
    Verify(!HasCycles(c));
    Verify(!HasCycles(d));

    Lock(a, c);
    Verify(!HasCycles(a));
    Verify(!HasCycles(b));
    Verify(!HasCycles(c));
    Verify(!HasCycles(d));

    Lock(b, d);
    Verify(!HasCycles(a));
    Verify(!HasCycles(b));
    Verify(!HasCycles(c));
    Verify(!HasCycles(d));

    Lock(a, b, c);
    Verify(!HasCycles(a));
    Verify(!HasCycles(b));
    Verify(!HasCycles(c));
    Verify(!HasCycles(d));

    Lock(a, b, d);
    Verify(!HasCycles(a));
    Verify(!HasCycles(b));
    Verify(!HasCycles(c));
    Verify(!HasCycles(d));

    Lock(a, b, c, d);
    Verify(!HasCycles(a));
    Verify(!HasCycles(b));
    Verify(!HasCycles(c));
    Verify(!HasCycles(d));

    Lock(a, b, d, c);
    Verify(HasCycles(a));
    Verify(HasCycles(b));
    Verify(HasCycles(c));
    Verify(HasCycles(d));
}


template<class T>
struct LockMany : boost::noncopyable
{
    // No ownership here to prevent cycles causing infinite recursion on destruction.
    typedef Node<Mutex*> MutexNode;

    LockMany() :
        mRootNode(),
        mLastNode(&mRootNode)
    {
    }

    ~LockMany()
    {
        unlockAll();
    }

    void lock(Mutex & inMutex)
    {
        MutexNode * node = getMutexNode(&inMutex);
        mLastNode->insert(node);
        mLastNode = node;
    }

    void unlockAll()
    {
        mLastNode = &mRootNode;
    }

    MutexNode * getMutexNode(Mutex * inMutex)
    {
        typename MutexNodes::iterator it = mMutexNodes.find(inMutex);
        if (it == mMutexNodes.end())
        {
            MutexNode * theMutexNode = new MutexNode(inMutex);
            mMutexNodes.insert(std::make_pair(inMutex, theMutexNode));
            return theMutexNode;
        }
        return it->second.get();
    }

    typedef std::map<Mutex*, boost::shared_ptr<MutexNode> > MutexNodes;
    MutexNodes mMutexNodes;
    MutexNode mRootNode;
    MutexNode * mLastNode;
};


void testLockMany()
{
    LockMany<char> locker;

    Mutex a('1');
    Mutex b('2');
    Mutex c('3');

    // Ok
    locker.lock(a);
    locker.lock(b);
    Verify(!HasCycles(locker.mRootNode));
    Verify(!HasCycles(*locker.getMutexNode(&a)));
    Verify(!HasCycles(*locker.getMutexNode(&b)));
    locker.unlockAll();

    // Ok
    locker.lock(a);
    locker.lock(b);
    Verify(!HasCycles(locker.mRootNode));
    Verify(!HasCycles(*locker.getMutexNode(&a)));
    Verify(!HasCycles(*locker.getMutexNode(&b)));
    locker.unlockAll();

    // Ok
    locker.lock(b);
    locker.lock(c);
    Verify(!HasCycles(locker.mRootNode));
    Verify(!HasCycles(*locker.getMutexNode(&a)));
    Verify(!HasCycles(*locker.getMutexNode(&b)));
    Verify(!HasCycles(*locker.getMutexNode(&c)));
    locker.unlockAll();

    // Ok
    locker.lock(a);
    locker.lock(c);
    Verify(!HasCycles(locker.mRootNode));
    Verify(!HasCycles(*locker.getMutexNode(&a)));
    Verify(!HasCycles(*locker.getMutexNode(&b)));
    Verify(!HasCycles(*locker.getMutexNode(&c)));
    locker.unlockAll();

    // Test inconsistent locking order here:
    locker.lock(b);
    locker.lock(a);
    Verify(HasCycles(locker.mRootNode));
    Verify(HasCycles(*locker.getMutexNode(&a)));
    Verify(HasCycles(*locker.getMutexNode(&b)));
    Verify(!HasCycles(*locker.getMutexNode(&c))); // no cycles here
    locker.unlockAll();
}


int main()
{
    testNode();
    testLockMany();
    std::cout << "Everything is OK." << std::endl << std::flush;
    return 0;
}
