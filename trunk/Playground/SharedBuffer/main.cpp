#include <algorithm>
#include <iostream>
#include <new>
#include <assert.h>
#include <stdint.h>
#include <string.h>


template<typename T>
struct SharedSegment
{
    typedef T value_type;


    SharedSegment() : mData()
    {
        std::cout << "SharedSegment default constructed." << std::endl;
    }

    // Creates a SharedSegment which contains a copy of the data.
    SharedSegment(const T* inData, uint16_t inSize) :
        mData(CreateImpl(inSize)->data_ptr())
    {
        std::cout << "SharedSegment created with size " << size() << "." << std::endl;
        assert(size() == inSize);
        memcpy(data(), inData, size());
    }

    // Creates a SharedSegment which contains a copy of the data.
    SharedSegment(const T* inData, uint16_t inSize, uint16_t inCapacity) :
        mData(CreateImpl(inSize, inCapacity)->data_ptr())
    {
        std::cout << "SharedSegment created with size " << size() << "." << std::endl;
        assert(size() == inSize);
        memcpy(data(), inData, size());
        memset(end(), 0, capacity() - size());
    }

    SharedSegment(const SharedSegment& rhs) :
        mData(rhs.impl() ? rhs.impl()->copy() : nullptr)
    {
        std::cout << "SharedSegment copied with size " << size() << "." << std::endl;
    }

    SharedSegment& operator=(SharedSegment rhs)
    {
        std::cout << "SharedSegment copy-assinged with size " << size() << "." << std::endl;
        this->swap(rhs);
        return *this;
    }

    ~SharedSegment()
    {
        std::cout << "SharedSegment destroyed with size " << size() << "." << std::endl;
        if (impl())
        {
            impl()->discard();
        }
    }

    void swap(SharedSegment<T>& rhs)
    {
        std::swap(mData, rhs.mData);
    }

    void push_back(T t)
    {
        if (size() == 0)
        {
            if (capacity() == 0)
            {
                mData = CreateImpl(1, 64)->data_ptr();
            }
            else
            {
                SharedSegment<T>(data(), size(), 2 * capacity()).swap(*this);
            }
        }
        if (size() < capacity())
        {
            new (static_cast<void*>(data() + size())) T(std::move(t));
            impl()->grow(1);
        }
    }

    T* begin()
    {
        return mData;
    }

    const T* begin() const
    {
        return mData;
    }

    T* end()
    {
        return mData + size();
    }

    const T* end() const
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

    T* data()
    {
        return mData;
    }

    const T* data() const
    {
        return mData;
    }

private:
    struct Impl
    {
        Impl(uint16_t inSize, int16_t inCapacity, uint16_t inRefCount = 1) :
            mSize(inSize),
            mCapacity(inCapacity),
            mRefCount(inRefCount)
        {
            assert(mSize <= mCapacity);
        }

        T* copy()
        {
            mRefCount++;
            return data_ptr();
        }

        void discard()
        {
            if (--mRefCount == 0)
            {
                delete this;
            }
        }

        const T* data_ptr() const
        {
            return reinterpret_cast<const T*>(this + 1);
        }

        T* data_ptr()
        {
            return reinterpret_cast<T*>(this + 1);
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

    void make_unique()
    {
        Impl* new_impl = CreateImpl(size(), capacity());
        memcpy(new_impl.data_ptr(), data(), size());
        this->mData = new_impl.data_ptr();

    }

    static Impl* CreateImpl(uint16_t size, uint16_t capacity)
    {
        return new (malloc(capacity + sizeof(Impl))) Impl(size, capacity);
    }

    static Impl& GetImpl(T* str)
    {
        return *(reinterpret_cast<Impl*>(str) - 1);
    }

    static T* GetData(Impl& impl)
    {
        return reinterpret_cast<T*>(&impl + 1);
    }

    Impl* impl() const
    {
        return mData ? const_cast<Impl*>(reinterpret_cast<const Impl*>(mData) - 1) : nullptr;
    }

    T* mData;
};



int main()
{
    SharedSegment<int> a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    SharedSegment<int> b = a;
    b.push_back(4);
    b.push_back(5);
    b.push_back(6);
    auto c = b;
    a = c;
    c = a = b;
}
