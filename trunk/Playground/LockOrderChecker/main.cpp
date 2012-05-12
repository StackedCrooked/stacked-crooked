#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


namespace Detail {


template<typename T>
class Node;


template<typename T>
class Graph
{
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
    enum MakeNode { MakeNode_Yes, MakeNode_No };

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
private:
    friend class Graph<T>;
    friend class CycleDetector<T>;
    Node(const T & value) : mValue(value) { }
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    friend bool operator<(const Node<T> & lhs, const Node<T> & rhs)
    { return lhs.get() < rhs.get(); }

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
class CycleDetector
{
public:
    void push(const T & inValue)
    {
        graph().push(inValue);
        assert(!HasCycles());
    }

    void pop(const T & inValue)
    { graph().pop(inValue); }

protected:
    CycleDetector() : mId(GetNextId()++) { }

private:
    static bool HasCycles()
    { return FindCycle(graph().root()) != graph().root().end(); }

    static Graph<T> & graph()
    {
        static Graph<T> fGraph;
        return fGraph;
    }

    static unsigned & GetNextId()
    {
        static unsigned fId;
        return fId;
    }

    unsigned id() const { return mId; }

    friend bool operator==(const CycleDetector<T> & lhs, const CycleDetector<T> & rhs)
    { return lhs.id() == rhs.id(); }

    friend bool operator<(const CycleDetector<T> & lhs, const CycleDetector<T> & rhs)
    { return lhs.id() < rhs.id(); }

    unsigned mId;
};


class NoCycleDetector
{
public:
    template<typename T>
    void push(const T &) { }

    template<typename T>
    void pop(const T &) { }
};


} // namespace Detail


#ifndef NDEBUG
typedef Detail::CycleDetector<struct Mutex> MutexBase;
#else
typedef Detail::NoCycleDetector MutexBase;
#endif


struct Mutex : MutexBase
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


int main()
{
    std::cout << __FUNCTION__ << std::endl;

    Mutex a, b1, b2, c, d;
    {
        LOCK(a);
        LOCK(b1);
    }
    {
        LOCK(a);
        LOCK(b1);
    }
    {
        LOCK(a);
        LOCK(b2);
    }
    {
        LOCK(b1);
        LOCK(c);
    }
    {
        LOCK(b2);
        LOCK(c);
    }
    {
        LOCK(a);
        LOCK(c);
    }
    {
        LOCK(d);
    }
    {
        LOCK(a);
        LOCK(c);
        LOCK(d);
    }
    {
        LOCK(d);
        std::cout << "Intential lock inconsitency. Should assert!" << std::endl;
        LOCK(a); // Cycle! Assertion!
    }

    std::cout << std::endl;
    std::cout << "End of program." << std::endl;
    return 0;
}
