#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <assert.h>
#include <iterator>
#include <iostream>


template<class ContainerT>
struct BaseContainerPolicy
{
    typedef ContainerT Container;
};

template<class NodeT>
struct ContainerPolicy_Set : public BaseContainerPolicy<std::set<NodeT> >
{
    static void insert(Container & ioContainer, const NodeT & inValue)
    { ioContainer.insert(inValue); }

    static void clear(Container & ioContainer)
    { ioContainer.clear(); }
};

template<class NodeT>
struct ContainerPolicy_Vector : public BaseContainerPolicy<std::vector<NodeT> >
{
    static void insert(Container & ioContainer, const NodeT & inValue)
    { ioContainer.push_back(inValue); }

    static void clear(Container & ioContainer)
    { ioContainer.clear(); }
};

template <class PointeeType>
struct PointerPolicy_Normal
{
    typedef PointeeType* PointerType;

    static PointeeType* getRaw(PointerType p)
    {
        return p;
    }
};

template <class PointeeType>
struct PointerPolicy_Shared
{
    typedef boost::shared_ptr<PointeeType> PointerType;

    static PointeeType* getRaw(PointerType p)
    {
        return p.get();
    }
};


template <class DataType, template <class> class ContainerPolicy, template <class> class PointerPolicy>
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

    iterator begin() { return mChildren.begin(); }
    iterator end() { return mChildren.end(); }

    const_iterator begin() const { return mChildren.begin(); }
    const_iterator end() const { return mChildren.end(); }

    void addChild(This * inItem)
    { 
        ChildPtr item(inItem);
        ContainerPolicy<ChildPtr>::insert(mChildren, item);
    }

    void clearChildren()
    { ContainerPolicy<ChildPtr>::clear(mChildren); }

    const DataType & data() const
    { return mData; }

    void setData(const DataType & inData)
    { mData = inData; }

private:
    DataType mData;
    Container mChildren;
};


typedef GenericNode<std::string, ContainerPolicy_Vector, PointerPolicy_Normal> SimpleNode;
typedef GenericNode<std::string, ContainerPolicy_Set, PointerPolicy_Shared> SmartNode;


template<class Container>
void iterateContainer(Container & inContainer)
{
    {
        std::cout << "Non-const postincrement" << std::endl;
        Container::iterator it = inContainer.begin();
        while (it != inContainer.end())
        {
            Container::ChildPtr child = (*it);
            std::string data = (*it)->data();
            std::cout << data << std::endl;
            it++;
        }
    }
    {
        std::cout << "Non-const preincrement" << std::endl;
        Container::iterator it = inContainer.begin();
        while (it != inContainer.end())
        {
            Container::ChildPtr child = (*it);
            std::string data = (*it)->data();
            std::cout << data << std::endl;
            ++it;
        }
    }
}

template<class Container>
void iterateContainer(const Container & inContainer)
{
    {
        std::cout << "Const postincrement" << std::endl;
        Container::const_iterator it = inContainer.begin();
        while (it != inContainer.end())
        {
            Container::ChildPtr child = (*it);
            std::string data = (*it)->data();
            std::cout << data << std::endl;
            it++;
        }
    }
    {
        std::cout << "Const preincrement" << std::endl;
        Container::const_iterator it = inContainer.begin();
        while (it != inContainer.end())
        {
            Container::ChildPtr child = (*it);
            std::string data = (*it)->data();
            std::cout << data << std::endl;
            ++it;
        }
    }
}


template<class NodeConfig>
void testConfig()
{
    NodeConfig node;
    node.addChild(new NodeConfig("cup"));
    node.addChild(new NodeConfig("orange"));
    node.addChild(new NodeConfig("house"));
    node.addChild(new NodeConfig("zorro"));
    iterateContainer(node);

    const NodeConfig & constSimpleNode(node);
    iterateContainer(constSimpleNode);

    node.clearChildren();
}

template<class NodeT>
struct OrderByData
{
    bool operator () (NodeT lhs, NodeT rhs)
    {
        return lhs->data() < rhs->data();
    }
};

template<class NodeT>
struct ContainerPolicy_Set_CustomOrdering : public BaseContainerPolicy<std::set<NodeT, OrderByData<NodeT> > >
{
    static void insert(Container & ioContainer, const NodeT & inValue)
    { ioContainer.insert(inValue); }

    static void clear (Container & ioContainer)
    { ioContainer.clear(); }
};


int main()
{
    std::cout << "Vector, normal pointer" << std::endl;
    testConfig<GenericNode<std::string, ContainerPolicy_Vector, PointerPolicy_Normal> >();

    std::cout << "Vector, shared pointer" << std::endl;
    testConfig<GenericNode<std::string, ContainerPolicy_Vector, PointerPolicy_Shared> >();

    std::cout << "Set, normal pointer" << std::endl;
    testConfig<GenericNode<std::string, ContainerPolicy_Set, PointerPolicy_Normal> >();

    std::cout << "Set, shared pointer" << std::endl;
    testConfig<GenericNode<std::string, ContainerPolicy_Set, PointerPolicy_Shared> >();

    std::cout << "Set, normal pointer, custom ordening" << std::endl;
    testConfig<GenericNode<std::string, ContainerPolicy_Set_CustomOrdering, PointerPolicy_Normal> >();

    std::cout << "Set, shared pointer, custom ordening" << std::endl;
    testConfig<GenericNode<std::string, ContainerPolicy_Set_CustomOrdering, PointerPolicy_Shared> >();

    std::cout << "Type 'q' + ENTER to quit";
    char c;
    std::cin >> c;
    return 0;
}
