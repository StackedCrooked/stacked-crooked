#include <iostream>
#include <vector>

struct Value
{
    int x;
};

struct Delta
{       
    int dx;
};

void apply(std::vector<Value> & values, const Delta & delta)
{
    for (unsigned i = 0; i < values.size(); i++)
    {
        values[i].x += delta.dx;
    }
}

int main()
{
    std::vector<Value> values(2);
    
    Delta & d = reinterpret_cast<Delta&>(values[0]);
    d.dx = 1;
    
    apply(values, d);
    
    std::cout << values[1].x << std::endl;
}
