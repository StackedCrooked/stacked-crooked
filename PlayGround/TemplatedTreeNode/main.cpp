#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <assert.h>
#include <iterator>
#include <iostream>


template<class DataType>
struct ContainerPolicy_Set
{
    typedef std::set<DataType> Container;
    static void insert(Container & ioContainer, const DataType & inValue)
    {
        ioContainer.insert(inValue);
    }
};

template<class DataType>
struct ContainerPolicy_Vector
{
    typedef std::vector<DataType> Container;
    static void insert(Container & ioContainer, const DataType & inValue)
    {
        ioContainer.push_back(inValue);
    }
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


    // simple iterator
    class const_iterator : public std::iterator<std::forward_iterator_tag, This>
    {
    public:
        bool operator== (const const_iterator & rhs) const
        { return mIterator == rhs.mIterator; }
        
        bool operator!= (const const_iterator & rhs) const
        { return mIterator != rhs.mIterator; }

        // preincrement
        const_iterator operator++()
        {
            return const_iterator(++mIterator);
        }

        // postincrement
        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        // return reference to class object
        const ChildPtr & operator*() { return *mIterator; }

        // return pointer to class object
        const ChildPtr * operator->() { return &(*mIterator); }

    private:
        friend class This;
        const_iterator(const typename Container::const_iterator & inIterator) : mIterator(inIterator) { }
        typename Container::const_iterator mIterator;
    };

    class iterator : public std::iterator<std::forward_iterator_tag, This>
    {
    public:
        bool operator== (const iterator & rhs) const
        { return mIterator == rhs.mIterator; }
        
        bool operator!= (const iterator & rhs) const
        { return mIterator != rhs.mIterator; }

        // preincrement
        iterator operator++()
        {
            return iterator(++mIterator);
        }

        // postincrement
        iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        // return reference to class object
        const ChildPtr & operator*() { return *mIterator; }

        // return pointer to class object
        const ChildPtr * operator->() { return &(*mIterator); }

    private:
        friend class This;
        iterator(const typename Container::iterator & inIterator) : mIterator(inIterator) { }
        typename Container::iterator mIterator;
    };

    iterator begin()
    {
        Container::iterator it = mChildren.begin();
        return iterator(it);
    }

    iterator end() { return iterator(mChildren.end()); }

    const_iterator begin() const { return const_iterator(mChildren.begin()); }

    const_iterator end() const { return const_iterator(mChildren.end()); }


    void addChild(This * inItem)
    { 
        ChildPtr item(inItem);
        ContainerPolicy<ChildPtr>::insert(mChildren, item);
    }

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
}


template<class NodeT>
struct ContainerPolicy_Set_CustomOrdering
{
    struct CustomOrder
    {
        bool operator () (NodeT lhs, NodeT rhs)
        {
            return lhs->data() < rhs->data();
        }
    };
    typedef std::set<NodeT, CustomOrder> Container;

    static void insert(Container & ioContainer, const NodeT & inValue)
    {
        ioContainer.insert(inValue);
    }
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

    std::cout << "Type 'q' + ENTER to quit";
    char c;
    std::cin >> c;
    return 0;
}
