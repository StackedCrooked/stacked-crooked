#ifndef POOL_H
#define POOL_H


#include <cstddef>
#include <vector>


struct Pool
{
    // Returns a refernce to the most recently created pool (top of the stack).
    static Pool & Get();

    Pool(std::size_t inSize);

    ~Pool();

    template<class T>
    inline T * allocate(std::size_t n = 1)
    {
        std::size_t size = n * sizeof(T);
        if (mUsed + size > capacity())
        {
            throw std::bad_alloc();
        }

        T * result = reinterpret_cast<T*>(mData.data() + mUsed);
        mUsed += size;
        return result;
    }

    template<class T>
    inline void deallocate(T *, size_t n = 1)
    {
        mFreed += n * sizeof(T);
        if (mFreed == mUsed)
        {
            mFreed = mUsed = 0;
        }
    }

    inline std::size_t used() const { return mUsed; }

    inline std::size_t capacity() const { return mSize; }

private:
    Pool(const Pool&);
    Pool& operator=(const Pool&);

    typedef std::vector<unsigned char> Data;
    Data mData;
    std::size_t mSize;
    std::size_t mUsed;
    std::size_t mFreed;
    static std::vector< Pool* > sInstances;
};


#endif // POOL_H
