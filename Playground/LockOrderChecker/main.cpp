#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
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


template<typename T>
class Node;


template<typename T>
class Graph
{
private:
    enum MakeNode { MakeNode_Yes, MakeNode_No };
public:
    typedef std::shared_ptr< Node<T> > NodePtr;

    Graph() :
        mRootNode(),
        mLastNodes()
    {
    }

    void push(const T & inValue)
    {
        Node<T> * node = get(inValue, MakeNode_Yes);
        if (!mLastNodes.empty())
        {
            mLastNodes.back()->insert(node);
        }
        mLastNodes.push_back(node);
    }

    void pop(const T & inValue)
    {
        Node<T> * node = get(inValue, MakeNode_No);
        assert(node == mLastNodes.back());
        mLastNodes.pop_back();
    }

    Node<T> & root()
    {
        return *mRootNode;
    }

private:
    Node<T> * get(const T & inValue, MakeNode inMakeNode)
    {
        auto it = mNodes.find(inValue);
        if (it == mNodes.end() && inMakeNode == MakeNode_Yes)
        {
            NodePtr ptr(new Node<T>(inValue));
            if (mNodes.empty())
            {
                mRootNode = ptr;
            }
            mNodes.insert(std::make_pair(inValue, ptr));
            return ptr.get();
        }
        return it->second.get();
    }

    NodePtr mRootNode;
    std::vector<Node<T> * > mLastNodes;
    std::map<T, NodePtr> mNodes;
};


template <class T>
struct CycleDetector;


template <class T>
struct Node : std::set<Node<T>*>
{
    T & get() { return mValue; }
    const T & get() const { return mValue; }

    std::ostream & print(std::ostream & os, unsigned depth = 0, unsigned limit = 8) const
    {
        if (depth == limit)
        {
            return os;
        }

        static std::string tab(" ");
        std::string indent(depth * tab.size(), ' ');

        os << indent << mValue;
        for (auto & child : *this)
        {
            os << std::endl << indent << tab;
            child->print(os, depth + 1);
        }
        return os;
    }

private:
    friend class Graph<T>;
    friend class CycleDetector<T>;
    Node(const T & value) : mValue(value) { }
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    friend bool operator<(const Node<T> & lhs, const Node<T> & rhs)
    { return lhs.get() < rhs.get(); }


    friend std::ostream & operator<<(std::ostream & os, const Node<T> & node)
    { return node.print(os); }

    T mValue;
};


template<class T>
typename Node<T>::const_iterator FindCycle(const Node<T> & inNode,
                                           std::vector<const T*> inPreceding = std::vector<const T*>()) // by value!
{
    typename Node<T>::const_iterator it = inNode.begin(), end = inNode.end();
    for (; it != end; ++it)
    {
        const Node<T> & child = **it;
        if (std::find_if(inPreceding.begin(), inPreceding.end(), [&](const T * value) { return *value == child.get(); }) != inPreceding.end())
        {
            return it;
        }
        else
        {
            inPreceding.push_back(&inNode.get());

            typename Node<T>::const_iterator childIt = FindCycle(child, inPreceding);
            if (childIt != child.end())
            {
                return childIt;
            }
        }
    }
    return it;
}


template<typename T>
class WithId
{
public:
    WithId() : mId(Id()++) {}

    unsigned id() const { return mId; }

private:
    static unsigned & Id()
    {
        static unsigned fId;
        return fId;
    }

    unsigned mId;
};



template<typename T>
class CycleDetector : WithId<T>
{
public:
    void push(const T & inValue)
    {
        graph().push(inValue);
    }

    void pop(const T & inValue)
    {
        graph().pop(inValue);
    }

    static void Print(std::ostream & os)
    {
        os << graph().root();
    }

    static bool HasCycles()
    {
        return FindCycle(graph().root()) != graph().root().end();
    }

private:
    static Graph<T> & graph()
    {
        static Graph<T> fGraph;
        return fGraph;
    }

    friend bool operator==(const CycleDetector<T> & lhs, const CycleDetector<T> & rhs)
    {
        return lhs.id() == rhs.id();
    }

    friend bool operator<(const CycleDetector<T> & lhs, const CycleDetector<T> & rhs)
    {
        return lhs.id() < rhs.id();
    }

    friend std::ostream & operator<<(std::ostream & os, const CycleDetector<T> & inCycleDetector)
    {
        return os << inCycleDetector.id();
    }

};


template<typename T>
class NoCycleDetector : WithId<T>
{
public:
    void push(const T &) { }
    void pop(const T &) { }
};


template<typename T>
struct Identity
{
    typedef T Type;
};


template< template< template<class> class> class T>
bool HasCyclesImpl(const Identity< T<CycleDetector> > &)
{
    return T<CycleDetector>::HasCycles();
}


template< template< template<class> class> class T>
bool HasCyclesImpl(const Identity< T<NoCycleDetector> > &)
{
    return false;
}


template<typename T>
bool HasCycles()
{
    return HasCyclesImpl(Identity<T>());
}


template<template<class> class CycleDetectorType>
struct BasicMutex : CycleDetectorType< BasicMutex<CycleDetectorType> >
{
    void lock()
    {
        this->push(*this);
    }

    void unlock()
    {
        this->pop(*this);
    }
};


#ifndef NDEBUG
typedef BasicMutex<CycleDetector> Mutex;
#else
typedef BasicMutex<NoCycleDetector> Mutex;
#endif

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

    Mutex & mMutex;
};


#define CONCAT_HELPER(x, y) x ## y
#define CONCAT(x, y) CONCAT_HELPER(x, y)
#define LOCK(MTX) ScopedLock CONCAT(lock, __LINE__)(MTX)


void testMutex()
{
    std::cout << __FUNCTION__ << std::endl;

    {
        Mutex a, b1, b2, c, d;
        {
            LOCK(a);
            LOCK(b1);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(a);
            LOCK(b1);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(a);
            LOCK(b2);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(b1);
            LOCK(c);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(b2);
            LOCK(c);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(a);
            LOCK(c);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(d);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(a);
            LOCK(c);
            LOCK(d);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(d);
            LOCK(a);
            std::cout << std::endl;
            Verify(HasCycles<Mutex>());
        }

    }

    std::cout << std::endl;
}


int main()
{
    testMutex();
    std::cout << "End of program." << std::endl;
    return 0;
}
