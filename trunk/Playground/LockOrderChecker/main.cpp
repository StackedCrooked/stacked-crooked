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
public:
    typedef std::shared_ptr< Node<T> > NodePtr;

    Graph() :
        mRootNode(),
        mLastNode()
    {
    }

    void insert(const T & inValue)
    {
        Node<T> * node = get(inValue);
        if (mLastNode)
        {
            mLastNode->insert(node);
        }
        mLastNode = node;
    }

    Node<T> & root()
    {
        return *mRootNode;
    }

private:
    Node<T> * get(const T & inValue)
    {
        auto it = mNodes.find(inValue);
        if (it == mNodes.end())
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
    Node<T> * mLastNode;
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
    void insert(const T & inValue)
    {
        graph().insert(inValue);
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
bool HasCycles()
{
    return T::HasCycles();
}


struct Mutex : CycleDetector<Mutex>
{
    void lock()
    {
        this->insert(*this);
    }

    void unlock()
    {
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


void testMutex()
{
    std::cout << __FUNCTION__ << std::endl;

    {
        Mutex m1, m2;
        {
            LOCK(m1);
            LOCK(m2);
            Mutex::Print(std::cout);
            std::cout << std::endl;
            Verify(!HasCycles<Mutex>());
        }
        {
            LOCK(m2);
            LOCK(m1);
            Mutex::Print(std::cout);
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
