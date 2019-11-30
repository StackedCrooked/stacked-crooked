#include "Input.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>
#include <memory>
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
        auto result = 1;

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


void print_node(const Node& node, int indent = 0)
{
    std::cout
        << std::setw(12) << std::left << node.id() << ":"
        << std::string(indent, ' ')
        << node.name()
        << std::endl;
    for (auto& el : node.mChildren)
    {
        print_node(*el.second, indent + 4);
    }
}


void build_tree()
{
    for (const auto& pair : gInput)
    {
        auto parent_id = pair.first;
        auto child_id = pair.second;


        Node& parent_node = obtain_node(parent_id);

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

    std::cout << "Input: " << std::endl;
    for (auto& el : gInput)
    {
        std::cout << el.first << " " << el.second << std::endl;
    }

    std::cout << std::endl;

    build_tree();


    std::cout << "gAllNodes.size=" << gAllNodes.size() << " gRootNode.num_elements=" << gRootNode.num_elements() << std::endl;
    print_node(gRootNode);

    std::cout << "Not connected:\n";
    for (auto& el : gAllNodes)
    {
        Node& node = *el.second;
        if (!node.is_connected())
        {
            std::cout << "    [x] " << std::setw(12) << std::left << node.id() << " (" << node.name() << ")\n";
        }
    }

    verify_results();

    std::cout << std::endl;

}

