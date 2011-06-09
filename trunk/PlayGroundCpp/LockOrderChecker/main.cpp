#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>


template<class ValueType>
struct ContainerPolicy_Set
{
    typedef std::set<ValueType> Container;
    static void insert(Container & ioContainer, const ValueType & inValue)
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


template<class ValueType>
struct ContainerPolicy_Vector
{
    typedef std::vector<ValueType> Container;
    static void insert(Container & ioContainer, const ValueType & inValue)
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


template <class DataType,
          template <class> class ContainerPolicy,
          template <class> class PointerPolicy>
class GenericNode
{
public:
    GenericNode() { }

    GenericNode(const DataType & inData) : mData(inData) { }

    typedef GenericNode<DataType, ContainerPolicy, PointerPolicy> This;
    typedef typename PointerPolicy<This>::PointerType ChildPtr;
    typedef typename ContainerPolicy<ChildPtr>::Container Container;

    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;

    ~GenericNode()
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

    /**
     * Takes ownership.
     */
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
         template<class> class C,
         template<class> class P>
bool HasCycles(const GenericNode<T, C, P> & inNode,
               typename GenericNode<T, C, P>::Container inPreviousNodes,
               std::size_t inRecursionDepth)
{
    if (inRecursionDepth >= 10)
    {
        return false;
    }

    typedef GenericNode<T, C, P> Node;
    typedef typename Node::Container Container;

    if (!inNode.empty())
    {
        for (typename Container::const_iterator it = inNode.begin(),
                                                end = inNode.end();
             it != end;
             ++it)
        {
            GenericNode<T, C, P> & child = **it;
            if (inPreviousNodes.find(&child) != inPreviousNodes.end())
            {
                return true;
            }
            else
            {
                inPreviousNodes.insert(&const_cast<Node&>(inNode));
                if (HasCycles(child, inPreviousNodes, inRecursionDepth + 1))
                {
                    return true;
                }
            }
        }
    }
    return false;
}


template<class T, template<class> class C, template<class> class P>
bool HasCycles(const GenericNode<T, C, P> & inNode)
{
    typename GenericNode<T, C, P>::Container previousNodes; // empty for now
    return HasCycles(inNode, previousNodes, 1);
}


struct Mutex
{
    Mutex(char inChar) : mChar(inChar) {}
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


int main()
{
    typedef GenericNode<Mutex, ContainerPolicy_Set, PointerPolicy_Normal_NoOwnership> MutexNode;
    MutexNode a('a');
    MutexNode b('b');
    MutexNode c('c');
    MutexNode d('d');

    Lock(a, b);
    assert(!HasCycles(a));
    assert(!HasCycles(b));
    assert(!HasCycles(c));
    assert(!HasCycles(d));

    Lock(a, c);
    assert(!HasCycles(a));
    assert(!HasCycles(b));
    assert(!HasCycles(c));
    assert(!HasCycles(d));

    Lock(a, b, c);
    assert(!HasCycles(a));
    assert(!HasCycles(b));
    assert(!HasCycles(c));
    assert(!HasCycles(d));

    Lock(a, b, d);
    assert(!HasCycles(a));
    assert(!HasCycles(b));
    assert(!HasCycles(c));
    assert(!HasCycles(d));

    Lock(a, b, c, d);
    assert(!HasCycles(a));
    assert(!HasCycles(b));
    assert(!HasCycles(c));
    assert(!HasCycles(d));

    Lock(a, b, d, c);
    assert(HasCycles(a));
    assert(HasCycles(b));
    assert(HasCycles(c));
    assert(HasCycles(d));


    std::cout << "Done!" << std::endl;
    return 0;
}
