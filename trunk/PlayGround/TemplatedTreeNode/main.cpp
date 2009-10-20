#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <assert.h>
#include <iterator>

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


int main()
{
    SimpleNode simpleNode;
    simpleNode.addChild(new SimpleNode("test1"));
    simpleNode.addChild(new SimpleNode("test2"));
    //SimpleNode * a = simpleNode.getChild(0);
    //assert(a->data() == "test1");
    //const SimpleNode * b = static_cast<const SimpleNode>(simpleNode).getChild(1);
    //assert(b->data() == "test2");

    SmartNode smartNode;
    smartNode.addChild(new SmartNode("test3"));
    smartNode.addChild(new SmartNode("test4"));
    //SmartNode::iterator it = smartNode.begin();
    //SmartNode * c = *it;
    //assert(c->data() == "test3");
    return 0;
}
