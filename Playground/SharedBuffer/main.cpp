#include <algorithm>
#include <new>
#include <assert.h>
#include <stdint.h>


template<typename T>
struct SharedSegment
{
    typedef T value_type;
    typedef const T* const_iterator;


    SharedSegment() : mCharPtr() {}

    // Creates a SharedSegment which contains a copy of the data.
    SharedSegment(char* inData, uint16_t inSize) :
        mCharPtr(CreateImpl(inSize)->data_ptr())
    {
        assert(size() == inSize);
        memcpy(data(), inData, size());
    }

    SharedSegment(const SharedSegment& rhs) :
        mCharPtr(rhs.mCharPtr ? rhs.impl().increment() : nullptr)
    {
    }

    SharedSegment& operator=(SharedSegment rhs)
    {
        std::swap(mCharPtr, rhs.mCharPtr);
    }

    ~SharedSegment()
    {
        if (mCharPtr)
        {
            impl().decrement();
        }
    }

    void push_back(T t)
    {
        if (size() < capacity())
        {
            new (data() + size()) T(std::move(t));
            impl().mSize++;
        }
        assert(!"Capacity exceeded.");  // TODO: grow
    }

    const_iterator begin() const
    {
        return mCharPtr;
    }

    const_iterator end() const
    {
        return mCharPtr + size();
    }

    int size() const
    {
        return impl().mSize;
    }

    int capacity() const
    {
        return impl().mCapacity;
    }

    bool empty() const
    {
        return !size();
    }

    const_iterator data() const
    {
        return mCharPtr;
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

        T* increment()
        {
            mRefCount++;
            return data_ptr();
        }

        void decrement()
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

        uint16_t mSize;
        uint16_t mCapacity;
        uint16_t mRefCount;
        // 16-bits padding which may later be used for some purpose
    };

    void make_unique()
    {
        Impl* new_impl = CreateImpl(size(), capacity());
        memcpy(new_impl.data_ptr(), data(), size());
        this->mCharPtr = new_impl.data_ptr();

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

    const Impl& impl() const
    {
        return *(reinterpret_cast<const Impl*>(mCharPtr) - 1);
    }

    Impl& impl()
    {
        return *(reinterpret_cast<Impl*>(mCharPtr) - 1);
    }

    T* impl_copy()
    {
        return impl().increment();
    }

    T* mCharPtr;
};



int main()
{
    SharedSegment<char> data;
}
