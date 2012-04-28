#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>


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
struct Node_ConstIterator;

template<typename T>
struct Node
{
    typedef std::vector<Node<T> > container_type;

    Node(const T & t = T());

    const T & value() const;

    T & value();

    Node<T> & add(const T & value);

    const std::vector<Node<T> > & children() const;
    std::vector<Node<T> > & children();

    typedef ::Node_ConstIterator<T> const_iterator;
    const_iterator begin() const;
    const_iterator end() const;

private:
    const container_type & container() const;
    container_type & container();

    struct Impl;
    std::shared_ptr<Impl> mImpl;
};


template<typename T>
struct Node<T>::Impl
{
    typedef std::vector<Node<T> > Children;

    Impl(const T & value) : mValue(value)
    {
    }

    ~Impl()
    {
    }

    const T & value() const { return mValue; }
    T & value() { return mValue; }

    const Children & children() const { return mChildren; }
    Children & children() { return mChildren; }

    T mValue;
    Children mChildren;
};


template<typename T>
struct Node_ConstIterator
{
    typedef typename Node<T>::container_type        container_type;
    typedef typename container_type::const_iterator const_iterator;

    Node_ConstIterator(const container_type & c, const_iterator it) :
        mContainer(c),
        mIterator(it)
    {
    }

    Node_ConstIterator & operator++()
    {
        ++mIterator;
        return *this;
    }

    const Node<T> & operator*() const
    {
        const Node<T> & node = *mIterator;
        return node;
    }

    const container_type & mContainer;
    const_iterator mIterator;
};


template<typename T>
bool operator==(const Node_ConstIterator<T> & lhs, const Node_ConstIterator<T> & rhs)
{
    return lhs.mIterator == rhs.mIterator;
}


template<typename T>
bool operator!=(const Node_ConstIterator<T> & lhs, const Node_ConstIterator<T> & rhs)
{
    return lhs.mIterator != rhs.mIterator;
}


template<typename T>
Node<T>::Node(const T & t) :
    mImpl(new Impl(t))
{
}


template<typename T>
const typename Node<T>::container_type & Node<T>::container() const
{
    return container();
}


template<typename T>
typename Node<T>::container_type & Node<T>::container()
{
    return container();
}


template<typename T>
Node_ConstIterator<T> Node<T>::begin() const
{
    typedef typename Node_ConstIterator<T>::container_type container_type;
    const container_type & c = container();
    return Node_ConstIterator<T>(c, c.begin());
}


template<typename T>
Node_ConstIterator<T> Node<T>::end() const
{
    return Node_ConstIterator<T>(container(), container().end());
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
    return mImpl->value();
}


template<typename T>
T & Node<T>::value()
{
    return mImpl->value();
}


template<typename T>
const std::vector<Node<T> > & Node<T>::children() const
{
    return mImpl->_children;
}


template<typename T>
std::vector<Node<T> > & Node<T>::children()
{
    return mImpl->children();
}


int main()
{
    Node<int> root(0);
    root.add(1).add(2).add(3);
    Node<int>::const_iterator it = root.begin();
    Node<int>::const_iterator end = root.end();
    for (; it != end; ++it)
    {
        std::cout << (*it).value() << std::endl;
    }

}
