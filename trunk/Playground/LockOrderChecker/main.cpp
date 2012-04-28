#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>


template<class Node,
         class Preceding = std::vector<Node> >
const Node * FindCycle(const Node & node, Preceding preceding = Preceding())
{
    if (node.empty())
    {
        return nullptr;
    }

    if (std::find(preceding.begin(), preceding.end(), node) != preceding.end())
    {
        return &node;
    }

    preceding.push_back(node);

    for (const Node & child : node)
    {
        if (const Node * found = FindCycle(child, preceding))
        {
            return found;
        }
    }

    return nullptr;
}


template<typename T>
struct Node
{
    typedef std::vector<Node<T> > container_type;

    explicit Node(const T & t);

    const T & value() const;

    T & value();

    bool empty() const;

    Node<T> & add(const T & value);

    const std::vector<Node<T> > & children() const;
    std::vector<Node<T> > & children();

    struct const_iterator;
    const_iterator begin() const;
    const_iterator end() const;

    bool equals(const Node<T> & rhs) const;

private:
    const container_type & container() const;
    container_type & container();

    struct Impl;
    std::shared_ptr<Impl> mImpl;
};


template<typename T>
Node<T> MakeNode(const T & value)
{
    return Node<T>(value);
}


template<typename T>
struct Node<T>::Impl
{
    typedef std::vector<Node<T> > Children;

    explicit Impl(const T & value) : mValue(value)
    {
    }

    ~Impl()
    {
    }

    const T & value() const { return mValue; }
    T & value() { return mValue; }

    const Children & children() const { return mChildren; }
    Children & children() { return mChildren; }

    Node<T> & add(const T & value)
    {
        mChildren.push_back(MakeNode(value));
        return mChildren.back();
    }

    T mValue;
    Children mChildren;
};


template<typename T>
struct Node<T>::const_iterator
{
    typedef typename Node<T>::container_type        container_type;
    typedef typename container_type::const_iterator iterator_type;

    const_iterator(const container_type & c, iterator_type it) :
        mContainer(c),
        mIterator(it)
    {
    }

    const_iterator & operator++()
    {
        ++mIterator;
        return *this;
    }

    const Node<T> & operator*() const
    {
        const Node<T> & node = *mIterator;
        return node;
    }

    bool equals(const const_iterator & rhs) const
    {
        return mIterator == rhs.mIterator;
    }

    const container_type & mContainer;
    iterator_type mIterator;
};


template<typename ConstIterator>
bool operator==(const ConstIterator & lhs, const ConstIterator & rhs)
{
    return lhs.equals(rhs);
}


template<typename ConstIterator>
bool operator!=(const ConstIterator & lhs, const ConstIterator & rhs)
{
    return !lhs.equals(rhs);
}


template<typename T>
Node<T>::Node(const T & value = T()) :
    mImpl(new Impl(value))
{
}


template<typename T>
bool Node<T>::equals(const Node<T> & rhs) const
{
    return this->mImpl.get() == rhs.mImpl.get();
}



template<typename T>
bool Node<T>::empty() const
{
    return container().empty();
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
typename Node<T>::const_iterator Node<T>::begin() const
{
    return const_iterator(container(), container().begin());
}


template<typename T>
typename Node<T>::const_iterator Node<T>::end() const
{
    return const_iterator(container(), container().end());
}


template<typename T>
Node<T> & Node<T>::add(const T & value)
{
    return mImpl->add(value);
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

    FindCycle(root);

}
