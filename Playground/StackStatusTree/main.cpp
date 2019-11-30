#include "Input.h"
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>


struct Node
{
    explicit Node(int64_t id) :
        mId(id)
    {
    }

    int64_t id() const
    {
        return mId;
    }
    const std::string& name() const
    {
        if (mId == 0)
        {
            static std::string root_name = "root";
            return root_name;
        }
        return gDescriptions[mId];
    }

    bool is_connected() const
    {
        if (mId == 0)
        {
            return true;
        }

        return mParent && mParent->is_connected();
    }

    int64_t num_elements() const
    {
        // Note that the root node is not counted as an element
        auto result = mChildren.size();

        for (auto& el : mChildren)
        {
            result += el.second->num_elements();
        }

        return result;
    }

    int64_t mId = 0;
    Node* mParent = nullptr;
    std::map<int64_t, Node*> mChildren;
    bool mIsConnected = mId == 0;
    bool mIsLeaf = false;
};


Node gRootNode(0);
std::unordered_map<int64_t, std::unique_ptr<Node>> gAllNodes;
std::set<Node*> gOrphans;


Node* find_node(int64_t node_id)
{
    if (node_id == 0)
    {
        return &gRootNode;
    }

    auto it = gAllNodes.find(node_id);

    if (it == gAllNodes.end())
    {
        return nullptr;
    }

    return &*it->second;
}


Node& obtain_node(int64_t node_id)
{
    if (node_id == 0)
    {
        return gRootNode;
    }

    std::unique_ptr<Node>& node = gAllNodes[node_id];
    if (!node)
    {
        node.reset(new Node(node_id));
    }

    return *node;
}


void print_node(const Node& node, int indent)
{
    std::cout
        << std::string(indent, ' ')
        << node.name()
        << std::endl;
    for (auto& el : node.mChildren)
    {
        print_node(*el.second, indent + 4);
    }
}


void print_tree()
{
    std::cout << "TREE:" << std::endl;
    print_node(gRootNode, 0);
}


void print_input()
{
    std::cout << "Input: " << std::endl;
    for (auto& el : gInput)
    {
        std::cout << el.first << " " << el.second << std::endl;
    }

    std::cout << std::endl;

}


void count_orphans()
{
    for (auto& el : gAllNodes)
    {
        Node& node = *el.second;
        if (!node.is_connected())
        {
            gOrphans.insert(&node);
        }
    }
}


void build_tree()
{
    for (const auto& pair : gInput)
    {
        auto parent_id = pair.first;
        Node& parent_node = obtain_node(parent_id);

        auto child_id = pair.second;
        if (child_id == 0)
        {
            parent_node.mIsLeaf = true;
            continue;
        }

        Node& child_node = obtain_node(child_id);
        parent_node.mChildren[child_id] = &child_node;
        child_node.mParent = &parent_node;
    }
}


void verify_results()
{
    std::cout << "Verifying results: " << std::flush;

    for (const auto& pair : gInput)
    {
        auto parent_id = pair.first;
        auto child_id = pair.second;

        if (child_id == 0)
        {
            Node* leaf_node = find_node(parent_id);
            assert(leaf_node);
            assert(leaf_node->mIsLeaf);
            assert(leaf_node->mChildren.empty());
            continue;
        }

        Node* parent = find_node(parent_id);
        assert(parent);

        Node* child = find_node(child_id);
        assert(child);

        assert(parent->id() == parent_id);
        assert(child->id() == child_id);
        assert(parent->mChildren.count(child_id) == 1);
        assert(child->mParent == parent);

        if (parent_id == 0)
        {
            assert(parent == &gRootNode);
            assert(!parent->mParent);
        }
    }

    std::cout << "OK" << std::endl;
}


int main()
{
    std::random_shuffle(gInput.begin(), gInput.end());
    //print_input();
    build_tree();
    print_tree();
    verify_results();
    count_orphans();

    std::cout << std::endl;

    std::cout << "INFO:\n";
    std::cout << "  Input:   " << gInput.size() << " elements" << std::endl;
    std::cout << "  Tree:    " << gRootNode.num_elements() << " elements" << std::endl;
    std::cout << "  Orphans: " << gOrphans.size() << " elements" << std::endl;
}

