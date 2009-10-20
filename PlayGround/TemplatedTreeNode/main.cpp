#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <assert.h>


template <class PointeeType>
struct NormalPointerPolicy
{
    typedef PointeeType* PointerType;

    static PointeeType* getRaw(PointerType p)
    {
        return p;
    }
};

template <class PointeeType>
struct SharedPointerPolicy
{
    typedef boost::shared_ptr<PointeeType> PointerType;

    static PointeeType* getRaw(PointerType p)
    {
        return p.get();
    }
};


template <class DataType, template <class> class PointerPolicy>
class GenericNode
{
public:    
    GenericNode() { }

    GenericNode(const DataType & inData) : mData(inData) { }

    typedef GenericNode<DataType, PointerPolicy> This;

    typedef typename PointerPolicy<This>::PointerType ChildPtr;
    
    const This * getChild(size_t idx) const
    { return PointerPolicy<This>::getRaw(mChildren[idx]); }

    This * getChild(size_t idx)
    { return PointerPolicy<This>::getRaw(mChildren[idx]); }

    void addChild(This * inItem)
    { 
        ChildPtr item(inItem);
        mChildren.push_back(item);
    }

    const DataType & data() const
    { return mData; }

    void setData(const DataType & inData)
    { mData = inData; }

private:
    DataType mData;
    std::vector<ChildPtr> mChildren;
};

typedef GenericNode<std::string, NormalPointerPolicy> SimpleNode;
typedef GenericNode<std::string, SharedPointerPolicy> SmartNode;


int main()
{
    SimpleNode simpleNode;
    simpleNode.addChild(new SimpleNode("test1"));
    simpleNode.addChild(new SimpleNode("test2"));
    SimpleNode * a = simpleNode.getChild(0);
    assert(a->data() == "test1");
    const SimpleNode * b = static_cast<const SimpleNode>(simpleNode).getChild(1);
    assert(b->data() == "test2");

    SmartNode smartNode;
    smartNode.addChild(new SmartNode("test3"));
    smartNode.addChild(new SmartNode("test4"));
    SmartNode * c = smartNode.getChild(0);
    assert(c->data() == "test3");
    SmartNode * d = static_cast<const SmartNode>(smartNode).getChild(1);
    assert(d->data() == "test4");
    return 0;
}
