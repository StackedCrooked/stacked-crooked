#include <algorithm>
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
    const Node<T> & get(const T & inValue) const
    {
        return mNodes[inValue];
    }

    Node<T> & get(const T & inValue)
    {
        return mNodes[inValue];
    }

private:
    std::map<T, Node<T> > mNodes;
};


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
    typedef typename std::set<this_type>            container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    //! Returns the parent node.
    //! If this is the root node then null is returned.
    const Node<T> * parent() const { return _parent; }
    Node<T> * parent() { return _parent; }

    const Node<T> & root() const
    {
        if (!_parent) { return *this; }
        return parent()->root();
    }

    Node<T> & root() { return const_cast<Node<T>&>(static_cast<const Node<T>&>(*this).root()); }

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
        return const_cast<Node<T>&>(*_children.insert(Node<T>(this, item)).first);
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

    void print(std::ostream & os, unsigned depth = 0, unsigned limit = 16) const
    {
        if (depth == limit)
        {
            return;
        }

        static std::string tab(" ");
        std::string indent(depth * tab.size(), ' ');

        os << indent << get();
        for (auto & child : _children)
        {
            os << std::endl << indent << tab;
            child.print(os, depth + 1);
        }
    }

private:
    Node<T> * _parent;
    value_type _value;
    container_type _children;
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
        const Node<T> & child = *it;
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
        mId()
    {
    }

    void lock()
    {
        std::cout << "Lock " << sCurrentNode->get() << std::endl;

        // append current node
        sCurrentNode = &sCurrentNode->insert(id());
        assert(sCurrentNode->get() == id());
    }

    void unlock()
    {
        sCurrentNode = sCurrentNode->parent();
        std::cout << "Unlock " << sCurrentNode->get() << std::endl;
    }

    unsigned id() const { return mId.get(); }

private:
    static Node<unsigned> * sCurrentNode;
    UniqueNumber mId;
};


Node<unsigned> gRootNode;
Node<unsigned> * Mutex::sCurrentNode = &gRootNode;


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


void testFindCycles()
{
    {
        Node<int> node(nullptr, 0);
        node.insert(1).insert(0);
        std::cout << node << std::endl;
        Verify(HasCycles(node));
    }

    {
        Node<int> node(nullptr, 0);
        node.insert(1).insert(2);
        node.insert(2).insert(1);
        std::cout << node << std::endl;
        Verify(HasCycles(node));
    }
}


void testNode()
{
    Mutex m1, m2;

    {
        ScopedLock sl1(m1); (void)sl1;
        ScopedLock sl2(m2); (void)sl2;
    }

    Verify(!HasCycles(gRootNode));

    {
        ScopedLock sl1(m2); (void)sl1;
        ScopedLock sl2(m1); (void)sl2;
    }

    Verify(HasCycles(gRootNode));
}


int main()
{
    testFindCycles();
    //testNode();
    std::cout << "End of program." << std::endl;
    return 0;
}
