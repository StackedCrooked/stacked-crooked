#include <algorithm>
#include <cassert>
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

    Node(Node<T> * parent, const T & t);

    const Node<T> * parent() const;
    Node<T> * parent();

    const T & value() const;

    T & value();

    bool empty() const;

    void push_back(const T & value);

    const Node<T> & back() const;
    Node<T> & back();

    void pop_back();

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
struct Node<T>::Impl
{
    typedef std::vector<Node<T> > Children;

    explicit Impl(Node<T> * parent, const T & value) :
        mParent(parent),
        mValue(value)
    {
    }

    ~Impl()
    {
    }

    const Node<T> * parent() const { return mParent; }
    Node<T> * parent() { return mParent; }

    const Node<T> & back() const { return children().back(); }
    Node<T> & back() { return children().back(); }

    const T & value() const { return mValue; }
    T & value() { return mValue; }

    const Children & children() const { return mChildren; }
    Children & children() { return mChildren; }

    void push_back(const Node<T> & value) { children().push_back(value); }

    void pop_back() { children().pop_back(); }

    Node<T> * mParent;
    T mValue;
    Children mChildren;
};


template<typename T>
struct Node<T>::const_iterator
{
    typedef typename Node<T>::container_type        container_type;
    typedef typename container_type::const_iterator iterator_type;

    explicit const_iterator(iterator_type it) :
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
Node<T>::Node(Node<T> * parent, const T & value = T()) :
    mImpl(new Impl(parent, value))
{
}


template<typename T>
const Node<T> * Node<T>::parent() const
{
    return mImpl->parent();
}


template<typename T>
Node<T> * Node<T>::parent()
{
    return mImpl->parent();
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
    return mImpl->mChildren;
}


template<typename T>
typename Node<T>::container_type & Node<T>::container()
{
    return mImpl->mChildren;
}


template<typename T>
typename Node<T>::const_iterator Node<T>::begin() const
{
    return const_iterator(container().begin());
}


template<typename T>
typename Node<T>::const_iterator Node<T>::end() const
{
    return const_iterator(container().end());
}


template<typename T>
void Node<T>::push_back(const T & value)
{
    mImpl->push_back(Node<T>(this, value));
}


template<typename T>
const Node<T> & Node<T>::back() const
{
    return mImpl->back();
}


template<typename T>
Node<T> & Node<T>::back()
{
    return mImpl->back();
}


template<typename T>
void Node<T>::pop_back()
{
    mImpl->pop_back();
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


struct Mutex;
static Node<Mutex*> gRoot(nullptr);
static Node<Mutex*> * gCurrent = &gRoot;


struct Mutex
{
    Mutex(const std::string & inName) : mName(inName)
    {
    }

    Mutex(const Mutex &) = delete;
    Mutex & operator=(const Mutex &) = delete;

    const std::string & name() const { return mName; }

    void lock()
    {
        gCurrent->push_back(this);
        gCurrent = &gCurrent->back();
        assert(!FindCycle(gRoot));
    }

    void unlock()
    {
        assert(gCurrent->value() == this);
        gCurrent = gCurrent->parent();
    }

    std::string mName;
};


struct Lock
{
    Lock(Mutex & mutex) : mutex(mutex)
    {
        mutex.lock();
    }

    Lock(const Lock &) = delete;
    Lock & operator=(const Lock &) = delete;

    ~Lock()
    {
        mutex.unlock();
    }

    Mutex & mutex;
};


template<typename T>
std::ostream & operator<<(std::ostream & os, const Node<T> & node)
{
    static int fDepth = 0;
    std::string tab = std::string("  ");
    std::string indent = std::string(tab.size() * fDepth, ' ');
    os << std::endl << indent << node.value() << " ";
    for (const auto & child : node)
    {
        fDepth++;
        os << child;
        if (--fDepth == 0)
        {
            os << std::endl;
        }
    }
    return os;
}


std::ostream & operator<<(std::ostream & os, const Mutex * mutex)
{
    return os << (mutex ? mutex->name() : "null");
}


int main()
{
    Node<Mutex*> & theRoot = gRoot;
    Mutex m1("m1");
    Mutex m2("m2");
    Mutex m3("m3");
    Mutex m4("m4");
    {
        Lock l1(m1);
        Lock l2(m2);
    }
    {
        Lock l4(m4);
        Lock l3(m3);
    }
    {
        Lock l1(m1);
        Lock l4(m4);
    }
    std::cout << theRoot << std::endl;
    if (auto node = FindCycle(theRoot))
    {
        std::cout << "Found cycle at " << node->value()->name() << std::endl;
    }
    else
    {
        std::cout << "No cycles detected." << std::endl;
    }
}
