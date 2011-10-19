#include <algorithm>
#include <cstddef>

template<typename T>
struct Buffer
{
    Buffer(std::size_t n) :
        refcount(new std::size_t(1)),
        data(new T[n])
    {    
    }

    Buffer(const Buffer<T> & rhs) :
        refcount(rhs.refcount),
        data(rhs.data)
    {
        (*rhs.refcount)++;
    }

    Buffer<T>& operator=(Buffer<T> rhs)
    {
        std::swap(refcount, rhs.refcount);
        std::swap(data, rhs.data);
        return *this;
    }

    ~Buffer()
    {
        if (--(*refcount) == 0)
        {
            delete [] data;
            delete refcount;
        }
    }

private:
    mutable std::size_t * refcount;
    T * data;
};


typedef Buffer<int> Numbers;


int main()
{
    Numbers n1(10);
    Numbers n2(20);
    Numbers n3(30);
    n2 = n1 = n3 = n2 = n1 = n2;
}
