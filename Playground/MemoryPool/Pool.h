#ifndef POOL_H
#define POOL_H


#include <cstddef>
#include <vector>


template<typename T>
struct Pool
{
    Pool(std::size_t capacity = 100) :
        mData(capacity)
    {
    }

    inline T get()
    {
        T t;
        get(t);
        return t;
    }

    inline void get(T& t)
    {
        if (!mData.empty())
        {
            t = std::move(mData.back());
            mData.pop_back();
        }
    }

    inline void recycle(T&& t)
    {
        // this might trigger a reallocation
        mData.push_back(std::move(t));
    }

private:
    Pool(const Pool&);
    Pool& operator=(const Pool&);

    std::vector<T> mData;
};


#endif // POOL_H
