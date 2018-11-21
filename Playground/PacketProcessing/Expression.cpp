#include "Expression.h"
#include <iostream>


void Expression::print_true(int level) const
{
    std::cout << indent(level) << "<true/>\n";
}


void Expression::print_length(int level) const
{
    std::cout << indent(level) << "<bpf>len=" << mValue << "</bpf>\n";
}


void Expression::print_bpf(int level) const
{
    std::cout << indent(level) << "<bpf>" << mBPF->toString() << "</bpf>\n";
}


void Expression::print_binary(const char* op, int level) const
{
    std::cout << indent(level) << "<" << op << ">\n";
    for (const Expression& child : mChildren)
    {
        child.print(level + 2);
    }
    std::cout << indent(level) << "</" << op << ">\n";
}


std::string Expression::indent(int level)
{
    std::string result;
    for (auto i = 0; i != level; ++i)
    {
        result.push_back(' ');
    }
    return result;
}
