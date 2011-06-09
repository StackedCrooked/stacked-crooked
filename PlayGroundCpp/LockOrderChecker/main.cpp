#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>



template<class T>
struct Node : boost::noncopyable
{
    Node(const std::string & inName, const T & inT) :
        mName(inName),
        mT(inT)
    {
    }

    Node & append(Node & next)
    {
        mChildNodes.insert(&next);
        return next;
    }

    const std::string & name() const { return mName; }

    T & value() { return mT; }

    const T & value() const { return mT; }

    typedef Node<T> This;
    typedef std::set<This*> NodeSet;

    std::string mName;
    T mT;
    NodeSet mChildNodes;
};


template<class T>
bool HasCycles(const Node<T> & inNode, typename Node<T>::NodeSet & ioPreviousNodes, std::size_t inRecursionDepth)
{
    assert(inRecursionDepth < 100);

    typedef typename Node<T>::NodeSet NodeSet;

    const NodeSet & childNodes = inNode.mChildNodes;
    std::size_t childCount = childNodes.size();
    char firstLetter = inNode.name()[0];

    if (!childNodes.empty())
    {
        for (typename NodeSet::const_iterator it = childNodes.begin(),
                                              end = childNodes.end();
             it != end;
             ++it)
        {
            Node<T> & child = **it;
            if (ioPreviousNodes.find(&child) != ioPreviousNodes.end())
            {
                return true;
            }
            else
            {
                ioPreviousNodes.insert(&child);
                if (HasCycles(child, ioPreviousNodes, inRecursionDepth + 1))
                {
                    childCount ++;
                    firstLetter = 0;
                    return true;
                }
            }
        }
    }
    return false;
}


template<class T>
bool HasCycles(const Node<T> & inNode)
{
    typename Node<T>::NodeSet previousNodes; // empty for now
    return HasCycles(inNode, previousNodes, 1);
}


struct Mutex
{
};





int main()
{
    Node<int> a("a", 1);
    assert(!HasCycles(a));

    Node<int> b("b", 2);
    a.append(b);
    assert(!HasCycles(a));

    Node<int> c("c", 3);
    a.append(b).append(c);
    assert(!HasCycles(a));

    Node<int> d("d", 4);
    a.append(b).append(c).append(d);
    assert(!HasCycles(a));

    a.append(b).append(d);
    assert(HasCycles(a));
    assert(HasCycles(b));
    assert(!HasCycles(c));
    assert(!HasCycles(d));

    a.append(b).append(d).append(c);
    assert(HasCycles(a));
    assert(HasCycles(b));
    assert(HasCycles(c));
    assert(HasCycles(d));


    std::cout << "Done!" << std::endl;
    return 0;
}
