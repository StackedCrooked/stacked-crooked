#include "Input.h"
#include <iostream>
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
};


Node root_node(0);
std::unordered_map<int64_t, std::unique_ptr<Node>> all_nodes;


Node& obtain_node(int64_t node_id)
{
    if (node_id == 0)
    {
        return root_node;
    }

    std::unique_ptr<Node>& node = all_nodes[node_id];
    if (!node)
    {
        node.reset(new Node(node_id));
    }

    return *node;
}


void print_node(const Node& node, int indent = 0)
{
    std::cout << std::string(indent, ' ')
        //<< node.mId << " => "
        << node.name()
        << std::endl;
    for (auto& el : node.mChildren)
    {
        print_node(*el.second, indent + 4);
    }
}




int main()
{


    for (const auto& pair : gInput)
    {
        auto parent_id = pair.first;
        auto child_id = pair.second;

        if (child_id == 0)
        {
            continue;
        }

        Node& parent_node = obtain_node(parent_id);
        Node& child_node = obtain_node(child_id);

        parent_node.mChildren[child_id] = &child_node;
        child_node.mParent = &parent_node;
    }


    std::cout << "all_nodes.size=" << all_nodes.size() << " root_node.num_elements=" << root_node.num_elements() << std::endl;
    print_node(root_node);




    std::cout << "Not connected: ";
    for (auto& el : all_nodes)
    {
        Node& node = *el.second;
        if (!node.is_connected())
        {
            std::cout << node.mId << '(' << node.name() << ") ";
        }
    }

    std::cout << std::endl;

}

