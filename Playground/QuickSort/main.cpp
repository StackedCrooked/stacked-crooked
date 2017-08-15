#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

void print(const std::string& /*indent*/, int* b, int* e, int* pivot)
{
    //std::cout << indent;
    for (auto i = b; i != e; ++i)
    {
        if (i != pivot)
        {
            std::cout << "" << *i << " ";
        }
        else
        {
            std::cout << "[" << *i << "] ";
        }
    }
    std::cout << std::endl;
}

void partition(int* b, int* e, int n)
{

    std::string indent(4 * n, ' ');
    auto data = b;
    auto length = e - b;

    if (length <= 1)
    {
        //std::cout << indent << "End of partion. Length=" << length << std::endl;
        return;
    }

    auto left = data;
    auto pivot = data + length / 2;
    auto right = data + length - 1;
    std::cout << indent << "==== PARTION: ";
    print(indent, b, e, pivot);

    //std::cout << indent << "PIVOT=" << *pivot << std::endl;


    for (;;)
    {

        while (left < pivot && *left <= *pivot)
        {
            left++;
            //std::cout << indent << "left: data[" << (left - b) << "]=" << *left << std::endl;
        }

        while (right > pivot && *right >= *pivot)
        {
            right--;
            //std::cout << indent << "right: data[" << (right - b) << "]=" << *right << std::endl;
        }

        if (left == right)
        {
            partition(b, left, n + 1);
            partition(right + 1, e, n + 1);
            return;
        }

        std::cout << indent << "SWAP(" << *left << ", " << *right << "): ";

        std::swap(*left, *right);

        if (left == pivot)
        {
            pivot = right;
        }
        else if (right == pivot)
        {
            pivot = left;
        }

        print(indent, b, e, pivot);

    }

}


int main()
{
    std::srand(std::time(0));
    for (auto i = 0; i != 100; ++i)
    {
        std::cout << "=== i=" << i << " ===" << std::endl;
        std::vector<int> vec(i);
        for (auto& n : vec)
        {
            n = (&n - vec.data());// / (rand() % 3 == 0 ? 2 : 1);
        }

        auto sorted = vec;
        std::sort(sorted.begin(), sorted.end());

        std::random_shuffle(vec.begin(), vec.end());
        //std::reverse(vec.begin(), vec.end());

        auto b = vec.data();
        auto e = b + vec.size();

        partition(b, e, 0);


        std::cout << "RESULT: ";

        for (auto& n : vec)
        {
            std::cout << n << ' ';
        }

        std::cout << std::endl << std::endl;

        assert(std::is_sorted(vec.begin(), vec.end()));
        assert(sorted == vec);
    }

}
