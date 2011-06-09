#include <boost/shared_ptr.hpp>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>


struct Node;
struct NodeComparator;
typedef Node* NodePtr;
typedef std::set<NodePtr, NodeComparator> NodeSet;


struct NodeComparator
{
    bool operator()(NodePtr lhs, NodePtr rhs) const;
};


struct Node
{
    Node(const std::string & inName) :
        mName(inName)
    {
    }

    Node & append(Node & next)
    {
        mChildNodes.insert(&next);
        return next;
    }

    const std::string & name() const { return mName; }

    std::string mName;
    NodeSet mChildNodes;
};


bool NodeComparator::operator()(NodePtr lhs, NodePtr rhs) const
{
    return lhs->name() < rhs->name();
}


bool HasCycles(const Node & inNode, NodeSet & ioPreviousNodes, std::size_t inRecursionDepth)
{
    assert(inRecursionDepth < 100);

    const NodeSet & childNodes = inNode.mChildNodes;
    std::size_t childCount = childNodes.size();
    char firstLetter = inNode.name()[0];

    if (!childNodes.empty())
    {
        for (NodeSet::const_iterator it = childNodes.begin(),
                                     end = childNodes.end();
             it != end;
             ++it)
        {
            Node & child = **it;
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


bool HasCycles(const Node & inNode)
{
    NodeSet previousNodes; // empty for now
    return HasCycles(inNode, previousNodes, 1);
}


int main()
{
    Node a("a");
    assert(!HasCycles(a));

    Node b("b");
    a.append(b);
    assert(!HasCycles(a));

    Node c("c");
    a.append(b).append(c);
    assert(!HasCycles(a));

    Node d("d");
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
