#include <algorithm>
#include <vector>
#include <memory>
#include <boost/graph/directed_graph.hpp>


template<class FwdIt,
         class Node = typename std::iterator_traits<FwdIt>::value_type,
         class NodeList = std::vector<Node> >
FwdIt FindCycle(FwdIt it, FwdIt end, NodeList preceding = NodeList())
{
    if (it == end)
    {
        return it;
    }

    const Node & node(*it);

    if (std::find(begin(preceding), end(preceding), node) != end(preceding))
    {
        return it;
    }

    preceding.push_back(node);

    const auto & children = node.children();
    for (auto it = children.begin(), end = children.end(); it != end; ++it)
    {
        auto found = FindCycle(it, end, preceding);
        if (found != end)
        {
            return found;
        }
    }

    return end;
}


template<typename T>
struct Node
{
    Node(const T & t = T());

    const T & value() const;

    T & value();

    Node<T> & add(const T & value);

    const std::vector<Node<T> > & children() const;

    std::vector<Node<T> > & children();

    struct Impl;
    std::shared_ptr<Impl> _impl;
};


template<typename T>
struct Node<T>::Impl
{
    Impl(const T & value) : _value(value)
    {
    }

    ~Impl()
    {
    }

    T _value;
    std::vector<Node<T> > _children;
};


template<typename T>
Node<T>::Node(const T & t) :
    _impl(new Impl(t))
{
}


template<typename T>
Node<T> & Node<T>::add(const T & value)
{
    children().push_back(value);
    return children().back();
}


template<typename T>
const T & Node<T>::value() const
{
    return _impl->_value;
}


template<typename T>
T & Node<T>::value()
{
    return _impl->_value;
}


template<typename T>
const std::vector<Node<T> > & Node<T>::children() const
{
    return _impl->_children;
}


template<typename T>
std::vector<Node<T> > & Node<T>::children()
{
    return _impl->_children;
}


int main()
{
    Node<int> root(0);
    root.add(1).add(2).add(3);
    FindCycle(root.begin(), root.end());
}
