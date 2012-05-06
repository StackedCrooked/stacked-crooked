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


struct UniqueNumber
{
    UniqueNumber() :
        mNumber(GetUniqueNumber())
    {
    }

    unsigned get() const { return mNumber; }

private:
    static unsigned GetUniqueNumber()
    {
        static unsigned fUniqueNumber = 0;
        return fUniqueNumber++;
    }

    unsigned mNumber;
};


bool operator<(const UniqueNumber & lhs, const UniqueNumber & rhs)
{
    return lhs.get() < rhs.get();
}


template<typename T>
class Node;


template<typename T>
class Graph
{
public:
    typedef std::shared_ptr< Node<T> > NodePtr;

    Node<T> & get(const T & inValue)
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
            return *ptr;
        }
        return *it->second;
    }

    Node<T> & root()
    {
        return *mRootNode;
    }

private:
    NodePtr mRootNode;
    std::map<T, NodePtr> mNodes;
};


template <class T>
class Node
{
public:
    typedef T value_type;

    typedef Node<value_type>                        this_type;
    typedef typename std::set<this_type*>           container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    Node<T> & root() { return const_cast<Node<T>&>(static_cast<const Node<T>&>(*this).root()); }

    const_iterator begin() const
    {
        return mChildren.begin();
    }

    const_iterator end() const
    {
        return mChildren.end();
    }

    iterator begin()
    {
        return mChildren.begin();
    }

    iterator end()
    {
        return mChildren.end();
    }

    void insert(Node<T> & item)
    {
        mChildren.insert(&item);
    }

    size_t size() const
    {
        return mChildren.size();
    }

    bool empty() const
    {
        return mChildren.empty();
    }

    value_type & get()
    {
        return mValue;
    }

    const value_type & get() const
    {
        return mValue;
    }

    void set(const value_type & value)
    {
        mValue = value;
    }

    void print(std::ostream & os, unsigned depth = 0, unsigned limit = 16) const
    {
        if (depth == limit)
        {
            return;
        }

        static std::string tab(" ");
        std::string indent(depth * tab.size(), ' ');

        os << indent << get();
        for (auto & child : mChildren)
        {
            os << std::endl << indent << tab;
            child.print(os, depth + 1);
        }
    }

private:
    friend class Graph<T>;

    Node() {}

    Node(const value_type & value) :
        mValue(value)
    {
    }

    Node(const Node&);
    Node& operator=(const Node&);

    value_type mValue;
    container_type mChildren;
    UniqueNumber mId;
};


template<typename T>
bool operator<(const Node<T> & lhs, const Node<T> & rhs)
{
    return lhs.get() < rhs.get();
}


template<typename T>
std::ostream & operator<<(std::ostream & os, const Node<T> & node)
{
    node.print(os);
    return os;
}


template<class T>
typename Node<T>::const_iterator FindCycle(const Node<T> & inNode,
                                           std::vector<const T*> inPreceding = std::vector<const T*>())
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
bool HasCycles(const Node<T> & inNode)
{
    return FindCycle(inNode) != inNode.end();
}


struct Mutex
{
    Mutex() :
        mId(),
        mPreviousNode(nullptr)
    {
    }

    static Node<unsigned> & RootNode() { return sGraph.root(); }

    void lock()
    {
        mPreviousNode = sCurrentNode;

        // append current node
        Node<unsigned> & node = sGraph.get(id());
        if (sCurrentNode)
        {
            sCurrentNode->insert(node);
        }
        sCurrentNode = &node;
    }

    void unlock()
    {
        sCurrentNode = mPreviousNode;
    }

    unsigned id() const { return mId.get(); }

private:
    static Graph<unsigned> sGraph;
    static Node<unsigned> * sCurrentNode;
    UniqueNumber mId;
    Node<unsigned> * mPreviousNode;
};


Graph<unsigned> Mutex::sGraph;
Node<unsigned> * Mutex::sCurrentNode = 0;


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
    std::cout << __FUNCTION__ << std::endl;
    Mutex m1, m2;

    {
        ScopedLock sl1(m1); (void)sl1;
        ScopedLock sl2(m2); (void)sl2;
    }

    Verify(!HasCycles(Mutex::RootNode()));

    {
        ScopedLock sl1(m2); (void)sl1;
        ScopedLock sl2(m1); (void)sl2;
    }

    Verify(HasCycles(Mutex::RootNode()));
    std::cout << std::endl;
}


void testMutex()
{
    std::cout << __FUNCTION__ << std::endl;
    {
        Graph<int> graph;

        Node<int> & n1 = graph.get(1);
        Node<int> & n2 = graph.get(2);
        Node<int> & n3 = graph.get(3);
        Node<int> & n4 = graph.get(4);

        n1.insert(n2);
        Verify(!HasCycles(graph.root()));

        n1.insert(n3);
        Verify(!HasCycles(graph.root()));

        n3.insert(n2);
        Verify(!HasCycles(graph.root()));

        n2.insert(n4);
        Verify(!HasCycles(graph.root()));

        n4.insert(n3);
        Verify(HasCycles(graph.root()));
    }

    {
        Graph<int> graph;

        Node<int> & n1 = graph.get(1);
        Node<int> & n2 = graph.get(2);
        Node<int> & n3 = graph.get(3);

        n1.insert(n2);
        Verify(!HasCycles(graph.root()));

        n2.insert(n3);
        Verify(!HasCycles(graph.root()));

        n3.insert(n1);
        Verify(HasCycles(graph.root()));
    }
    std::cout << std::endl;
}


int main()
{
    testNode();
    testMutex();
    std::cout << "End of program." << std::endl;
    return 0;
}
