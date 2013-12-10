#include <algorithm>
#include <iostream>
#include <new>
#include <assert.h>
#include <stdint.h>


template<typename T>
struct SharedSegment
{
    typedef T value_type;
    typedef const T* const_iterator;


    SharedSegment() : mCharPtr() 
    {
        std::cout << "SharedSegment default constructed." << std::endl;
    }

    // Creates a SharedSegment which contains a copy of the data.
    SharedSegment(char* inData, uint16_t inSize) :
        mCharPtr(CreateImpl(inSize)->data_ptr())
    {
        std::cout << "SharedSegment created with size " << size() << "." << std::endl;
        assert(size() == inSize);
        memcpy(data(), inData, size());
    }

    SharedSegment(const SharedSegment& rhs) :
        mCharPtr(rhs.mCharPtr ? rhs.impl().increment() : nullptr)
    {
        std::cout << "SharedSegment copied with size " << size() << "." << std::endl;
    }

    SharedSegment& operator=(SharedSegment rhs)
    {
        std::cout << "SharedSegment copy-assinged with size " << size() << "." << std::endl;
        std::swap(mCharPtr, rhs.mCharPtr);
        return *this;
    }

    ~SharedSegment()
    {
        std::cout << "SharedSegment destroyed with size " << size() << "." << std::endl;
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
        return data() ? impl().mSize : 0;
    }

    int capacity() const
    {
        return data() ? impl().mCapacity : 0;
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

    Impl& impl() const
    {
        return const_cast<Impl&>(*(reinterpret_cast<const Impl*>(mCharPtr) - 1));
    }

    T* impl_copy()
    {
        return impl().increment();
    }

    T* mCharPtr;
};



int main()
{
    SharedSegment<char> a;
    SharedSegment<char> b = a;
    auto c = b;
    a = c;
    c = a = b;
}
