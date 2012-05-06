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


template<class T>
struct ContainerPolicy_Set
{
    typedef std::set<T> Container;
    static void insert(Container & ioContainer, const T & inValue)
    {
        ioContainer.insert(inValue);
    }

    static size_t size(const Container & ioContainer)
    {
        return ioContainer.size();
    }

    static bool empty(const Container & ioContainer)
    {
        return ioContainer.empty();
    }
};


template<class T>
struct ContainerPolicy_Vector
{
    typedef std::vector<T> Container;
    static void insert(Container & ioContainer, const T & inValue)
    {
        ioContainer.push_back(inValue);
    }

    static size_t size(const Container & ioContainer)
    {
        return ioContainer.size();
    }

    static bool empty(const Container & ioContainer)
    {
        return ioContainer.empty();
    }
};


template <class PointeeType>
struct PointerPolicy_Normal_NoOwnership
{
    typedef PointeeType * PointerType;

    static PointeeType * getRaw(PointerType p)
    {
        return p;
    }

    static void destroy(PointerType p) { }
};


template <class PointeeType>
struct PointerPolicy_Normal_WithOwnership
{
    typedef PointeeType * PointerType;

    static PointeeType * getRaw(PointerType p)
    {
        return p;
    }

    static void destroy(PointerType p)
    {
        delete p;
    }
};


template <class PointeeType>
struct PointerPolicy_Shared
{
    typedef boost::shared_ptr<PointeeType> PointerType;

    static PointeeType * getRaw(PointerType p)
    {
        return p.get();
    }

    static void destroy(PointerType p) { }
};


template <class TDataType,
          template <class> class ContainerPolicy,
          template <class> class PointerPolicy>
class node_t
{
public:
    typedef TDataType DataType;

    node_t() { }

    node_t(const DataType & inData) : mData(inData) { }

    typedef node_t<DataType, ContainerPolicy, PointerPolicy> This;
    typedef typename PointerPolicy<This>::PointerType ChildPtr;
    typedef typename ContainerPolicy<ChildPtr>::Container Container;

    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;

    ~node_t()
    {
        const_iterator it = this->begin(), endIt = this->end();
        for (; it != endIt; ++it)
        {
            PointerPolicy<This>::destroy(*it);
        }
    }

    iterator begin()
    {
        return mChildren.begin();
    }

    iterator end()
    {
        return mChildren.end();
    }

    const_iterator begin() const
    {
        return mChildren.begin();
    }

    const_iterator end() const
    {
        return mChildren.end();
    }

    void insert(This * inItem)
    {
        ChildPtr item(inItem);
        ContainerPolicy<ChildPtr>::insert(mChildren, item);
    }

    size_t size() const
    {
        return ContainerPolicy<ChildPtr>::size(mChildren);
    }

    bool empty() const
    {
        return ContainerPolicy<ChildPtr>::empty(mChildren);
    }

    DataType & data()
    {
        return mData;
    }

    const DataType & data() const
    {
        return mData;
    }

    void setData(const DataType & inData)
    {
        mData = inData;
    }

private:
    DataType mData;
    Container mChildren;
};


template<class T,
         template<class> class ContainerType,
         template<class> class PointerType>
bool HasCycles(const node_t<T, ContainerType, PointerType> & node,
               std::vector<const T*> node_path = std::vector<const T*>())
{
    typedef node_t<T, ContainerType, PointerType> Node;
    typedef typename Node::Container Container;

    if (!node.empty())
    {
        for (typename Container::const_iterator it = node.begin(), end = node.end(); it != end; ++it)
        {
            node_t<T, ContainerType, PointerType> & child = **it;
            if (std::find(node_path.begin(), node_path.end(), &child.data()) != node_path.end())
            {
                return true;
            }
            else
            {
                node_path.push_back(&node.data());

                // Previous nodes object is passed by value!
                // This is an important aspect of the algorithm!
                if (HasCycles(child, node_path))
                {
                    return true;
                }
            }
        }
    }
    return false;
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


void testMutex()
{
    // Defined the tree with PointerPolicy_Normal_NoOwnership to void deleting stack-allocated objects.
    typedef node_t<Mutex, ContainerPolicy_Set, PointerPolicy_Normal_NoOwnership> MutexNode;
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
    typedef node_t<Mutex*, ContainerPolicy_Set, PointerPolicy_Normal_NoOwnership> MutexNode;

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
    testMutex();
    testLockMany();
    std::cout << "Everything turned out better than expected." << std::endl << std::flush;
    return 0;
}
