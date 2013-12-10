#include <algorithm>
#include <iostream>
#include <new>
#include <assert.h>
#include <stdint.h>
#include <string.h>


struct SharedSegment
{
    typedef int value_type;


    SharedSegment() : mData()
    {
        print("SharedSegment default constructed.");
    }


    SharedSegment(uint16_t inSize) : mData(CreateImpl(inSize, 2 * inSize)->data_ptr())
    {
        memset(data(), 0, inSize);
        print("SharedSegment default constructed.");
    }

    // Creates a SharedSegment which contains a copy of the data.
    SharedSegment(const int* inData, uint16_t inSize) :
        mData(CreateImpl(inSize, 0)->data_ptr())
    {
        assert(size() == inSize);
        memcpy(mData, inData, size());
        print("SharedSegment created with size ");
    }

    // Creates a SharedSegment which contains a copy of the data.
    SharedSegment(const int* inData, uint16_t inSize, uint16_t inCapacity) :
        mData(CreateImpl(inSize, inCapacity)->data_ptr())
    {
        assert(size() == inSize);
        assert(inSize <= inCapacity);
        memcpy(mData, inData, inSize);
        memset(mData + inSize, 0, inCapacity - inSize);
        print("SharedSegment created with size ");
    }

    SharedSegment(const SharedSegment& rhs) :
        mData(rhs.mData)
    {
        assert(impl() == rhs.impl());
        if (rhs.impl())
        {
            rhs.impl()->acquire();
        }
        print("SharedSegment incremented");
    }

    SharedSegment& operator=(SharedSegment rhs)
    {
        auto& result = rhs.swap(*this);
        print("SharedSegment copy-assigned with size ");
        return result;
    }

    ~SharedSegment()
    {
        if (impl())
        {
            impl()->release();
            print("SharedSegment--");
        }
        print("SharedSegment destroyed with size ");
    }

    SharedSegment& swap(SharedSegment& rhs)
    {
        std::swap(mData, rhs.mData);
        print("SharedSegment swapped.");
        return *this;
    }

    void insert(int* b, int* e)
    {
        auto len = e - b;
        auto new_len = size() + len;
        if (new_len > capacity())
        {
            auto new_capacity = 2;
            while (new_capacity < new_len)
            {
                new_capacity *= 2;
            }
            SharedSegment(data(), new_len, new_capacity).swap(*this);
            print("insert results in realloc");
        }

        print("insert begin");
        std::copy(b, e, std::back_inserter(*this));
        print("insert complete");
    }

    void push_back(int t)
    {
        if (size() == capacity())
        {
            reserve(std::max(1, 2 * capacity()));
        }

        assert(impl());
        assert(size() < capacity());

        new (end()) int(std::move(t));
        impl()->grow(1);
        print("SharedSegment push_back results in regular growth.");
    }

    void reserve(uint16_t new_capacity)
    {
        if (new_capacity > capacity())
        {
            SharedSegment(data(), size(), new_capacity).swap(*this);
        }
    }

    int* begin()
    {
        return mData;
    }

    const int* begin() const
    {
        return mData;
    }

    int* end()
    {
        return mData + size();
    }

    const int* end() const
    {
        return mData + size();
    }

    uint16_t size() const
    {
        return impl() ? impl()->size() : 0;
    }

    uint16_t capacity() const
    {
        return impl() ? impl()->capacity() : 0;
    }

    bool empty() const
    {
        return !size();
    }

    int* data()
    {
        return mData;
    }

    const int* data() const
    {
        return mData;
    }

private:
    void print(const char* text)
    {
        std::cout << text << " size=" << size() << " capacity=" << capacity() << std::endl;
    }

    struct Impl
    {
        Impl(uint16_t inSize, int16_t inCapacity, uint16_t inRefCount = 1) :
            mSize(inSize),
            mCapacity(inCapacity),
            mRefCount(inRefCount)
        {
            assert(mSize <= mCapacity);
        }

        void acquire()
        {
            mRefCount++;
        }

        void release()
        {
            if (--mRefCount == 0)
            {
                delete this;
            }
        }

        int* data_ptr() const
        {
            return const_cast<int*>(reinterpret_cast<const int*>(this + 1));
        }

        uint16_t size() const
        {
            return mSize;
        }

        uint16_t capacity() const
        {
            return mCapacity;
        }

        void grow(int n)
        {
            mSize += n;
            assert(mSize <= mCapacity);
        }

    private:
        uint16_t mSize;
        uint16_t mCapacity;
        uint16_t mRefCount;
        // 16-bits padding which may later be used for some purpose
    };

    static Impl* CreateImpl(uint16_t size, uint16_t capacity)
    {
        return new (malloc(capacity + sizeof(Impl))) Impl(size, capacity);
    }

    Impl* impl() const
    {
        return mData ? const_cast<Impl*>(reinterpret_cast<const Impl*>(mData) - 1) : nullptr;
    }

    int* mData;
};



int main()
{
    SharedSegment a;
    for (int i = 0; i != 10; ++i)
    {
        a.push_back(i);
    }

    auto d = a;
    d.insert(a.begin(), a.end());
    d.insert(d.begin(), d.end());


}
