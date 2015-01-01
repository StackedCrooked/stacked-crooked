#include <boost/optional.hpp>
#include <boost/container/flat_map.hpp>
#include <mutex>
#include <scoped_allocator>
#include <string>
#include <vector>
#include <cstddef>
#include <vector>
#include <iostream>


// Create STL-compatible allocator for objects of type T using custom storage type.
namespace Detail {
namespace Inner {


template<typename Storage, typename T>
struct Allocator
{
    typedef T * pointer;
    typedef const T * const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template<typename U>
    struct rebind
    {
        typedef Allocator<Storage, U> other;
    };

    Allocator() = delete;

    Allocator(Storage& inStorage) :
        mStorage(&inStorage)
    {
        assert(mStorage);
    }

    Allocator(const Allocator& rhs) :
        mStorage(rhs.mStorage)
    {
        assert(mStorage);
    }

    template <typename U>
    Allocator(const Allocator<Storage, U>& rhs) : mStorage(rhs.mStorage)
    {
        assert(mStorage);
    }

    Allocator& operator=(const Allocator& rhs)
    {
        assert(mStorage);
        mStorage = rhs.mStorage;
        return *this;
    }

    ~Allocator()
    {
    }

    const T * address(const T& s) const { return &s; }
    T * address(T& r) const { return &r; }

    size_t max_size() const { return std::size_t(-1); }

    bool operator==(const Allocator&) const { return true; }
    bool operator!=(const Allocator& other) const { return !(*this == other); }

    template<typename U, typename ...Args>
    void construct(U* u, Args&& ...args)
    {
        new ((void*)u) U(std::forward<Args>(args)...);
    }

    void destroy(T * const p) const { p->~T(); }

    T * allocate(size_t n) const
    {
        assert(n);
        return static_cast<T*>(mStorage->allocate(n * sizeof(T)));
    }

    void deallocate(T * const p, const size_t n) const
    {
        assert(mStorage);
        mStorage->deallocate(p, n * sizeof(T));
    }

    template <typename U>
    T * allocate(const size_t n, const U*) const { return this->allocate(n); }

    Storage* mStorage;
};


} // namespace Inner


template<typename Storage, typename T>
using Allocator = std::scoped_allocator_adaptor<Inner::Allocator<Storage, T>>;


} // namespace Detail


/**
 * Decorator that adds mutex locking.
 */
template<typename Pool>
struct WithMutex
{
    void* allocate(std::size_t block_size)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        return mPool.allocate(block_size);
    }

    void deallocate(void* data, std::size_t block_size)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mPool.deallocate(data, block_size);
    }

private:
    std::mutex mMutex;
    Pool mPool;
};


/**
 * FixedFreeListPool is a pool that allocates fixed-size segments.
 * - allocate pops a block from the free-list (or uses malloc if empty)
 * - deallocate pushes the block on the free-list
 *
 * Requires less CPU than other pools but also makes no effort to have good memory locality.
 *
 * Recommended for large block sizes.
 */
struct SimplePool
{
    template<typename T>
    using Allocator = Detail::Allocator<SimplePool, T>;

    SimplePool() : mAllocs(), mPreviousMax()
    {
    }

    SimplePool(const SimplePool&) {}
    SimplePool& operator=(const SimplePool&) { return *this; }

    SimplePool(SimplePool&&) noexcept = default;
    SimplePool& operator=(SimplePool&&) noexcept = default;

    ~SimplePool()
    {
        if (!mSegments.empty())
        {
            std::cout << "SimplePool: Got " << mSegments.size() << " fixed-size blocks" << std::endl;
        }
        while (!mSegments.empty())
        {
            free(mSegments.back());
            mSegments.pop_back();
        }
    }

    void* allocate(std::size_t n)
    {
        if (mSegments.empty())
        {
            mAllocs += n;
            if (mAllocs >= 2 * mPreviousMax)
            {
                mPreviousMax = mAllocs;
                // LOG(Critical) << "FixedFreeListPool(" << this <<  ") is currently managing " << (mAllocs / n) << " blocks of size " << n << ". Total managed size: " << mAllocs << " bytes";
            }
            return malloc(n);
        }

        auto result = mSegments.back();
        mSegments.pop_back();
        return result;
    }

    void deallocate(void* data, std::size_t = 0)
    {
        mSegments.push_back(data);
    }

    //Mutex mMutex;
    std::vector<void*> mSegments;
    uint32_t mAllocs;
    uint32_t mPreviousMax;
};


/**
 * SmallObjectPool is a pool that allocates blocks grouped in chunks.
 * It requires more CPU than FixedFreeListPool but provides better memory locality.
 *
 * This is recommended for small block sizes.
 */
struct SmallObjectPool
{
    template<typename T>
    using Allocator = Detail::Allocator<SmallObjectPool, T>;

    SmallObjectPool() : mPreviousChunkCount()
    {
    }

    SmallObjectPool(SmallObjectPool&&) = default;
    SmallObjectPool& operator=(SmallObjectPool&&) = default;

    SmallObjectPool(const SmallObjectPool&) = delete;
    SmallObjectPool& operator=(const SmallObjectPool&) = delete;

    ~SmallObjectPool()
    {
        if (!mChunks.empty())
        {
            std::cout << "SmallObjectPool: Got " << mChunks.size() << " chunks (containing 255 blocks each)." << std::endl;
            // LOG(Critical) << "FixedPool: destroying " << mChunks.size() << " chunks.";
            while (!mChunks.empty())
            {
                Chunk& chunk = mChunks.back();
                chunk.cleanup();
                mChunks.pop_back();
            }
        }
    }

    void* allocate(std::size_t block_size)
    {
        //std::lock_guard<Mutex> lock(mMutex);

        if (mChunks.empty()) init(block_size);
        Chunk* chunk = findAvailableChunk();
        if (!chunk)
        {
            mChunks.resize(mChunks.size() + 1);

            mLastFree = nullptr;
            chunk = &mChunks.back();
            chunk->init(block_size);

            if (mChunks.size() >= 2 * mPreviousChunkCount)
            {
                mPreviousChunkCount = mChunks.size();
                // LOG(Critical) << "FixedChunkingPool(" << this << ") is currently managing " << mPreviousChunkCount << " chunks each containing 255 blocks of " << block_size << " bytes. Total managed size=" << (mPreviousChunkCount * 255);
            }
        }
        mLastAlloc = chunk;
        return chunk->allocate(block_size);
    }

    void deallocate(void* data, std::size_t block_size)
    {
        //std::lock_guard<Mutex> lock(mMutex);

        assert(!mChunks.empty());
        if (!mLastFree || !mLastFree->contains(data, block_size))
        {
            for (Chunk& chunk : mChunks)
            {
                if (chunk.contains(data, block_size))
                {
                    mLastFree = &chunk;
                    goto resume;
                }
            }
            // LOG(Critical) << "ChunkBasedFixedPool::deallocate: there no parent chunk for ptr=" << data << " size=" << block_size;
            assert(!"Invalid free");
        }

        resume:
        mLastFree->deallocate(data, block_size);
    }

private:
    struct Chunk
    {
        void init(std::size_t block_size)
        {
            auto chunk_size = block_size * std::uint8_t(-1);
            //// LOG(Critical) << "Chunk was allocated with size " << chunk_size << " for " << (chunk_size / block_size) << " objects of size " << block_size;
            mData = static_cast<uint8_t*>(malloc(chunk_size));
            mFirstFreeBlock = 0;
            mNumFreeBlocks = std::uint8_t(-1);

            for (std::uint8_t i = 0; i != std::uint8_t(-1); ++i)
            {
                getByIndex(block_size, i)[0] = i + 1;
            }
        }

        void cleanup()
        {
            free(mData);
            mData = nullptr;
        }

        void* allocate(std::size_t block_size)
        {
            assert(mNumFreeBlocks);
            if (!mNumFreeBlocks) return nullptr;
            auto result = getByIndex(block_size, mFirstFreeBlock);
            mFirstFreeBlock = result[0];
            --mNumFreeBlocks;
            return result;
        }

        void deallocate(void* data, std::size_t block_size)
        {
            assert(mData <= data && data <= mData + block_size * std::uint8_t(-1));
            auto block = static_cast<std::uint8_t*>(data);
            block[0] = mFirstFreeBlock;
            mFirstFreeBlock = getIndexOf(block_size, block);
            ++mNumFreeBlocks;
        }

        bool contains(void* data, std::size_t block_size)
        {
            return mData <= data && data < mData + std::uint8_t(-1) * block_size;
        }

        bool isFull() const
        {
            return mNumFreeBlocks == 0;
        }

        std::uint8_t* getByIndex(std::size_t block_size, std::size_t object_index)
        {
            assert(object_index <= std::uint8_t(-1));
            return mData + block_size * object_index;
        }

        std::size_t getIndexOf(std::size_t block_size, std::uint8_t* block)
        {
            auto difference = block - mData;
            assert(difference % block_size == 0);
            auto result = difference / block_size;
            assert(result <= std::uint8_t(-1));
            return result;
        }

        std::uint8_t* mData;
        std::uint8_t mFirstFreeBlock;
        std::uint8_t mNumFreeBlocks;
    };

    Chunk* findAvailableChunk()
    {
        if (!mLastAlloc->isFull())
        {
            return mLastAlloc;
        }

        for (Chunk& chunk : mChunks)
        {
            if (!chunk.isFull())
            {
                return &chunk;
            }
        }

        return nullptr;
    }

    void init(std::size_t block_size)
    {
        mChunks.resize(1);
        for (Chunk& chunk : mChunks)
        {
            chunk.init(block_size);
        }
        mLastAlloc = &mChunks.front();
        mLastFree = &mChunks.front();
        mChunkSize = block_size * uint8_t(-1);

        // LOG(Critical) << "FixedPool was created for objects of size " << block_size;
    }

    //Mutex mMutex;
    std::vector<Chunk> mChunks;
    Chunk* mLastAlloc;
    Chunk* mLastFree;
    uint32_t mChunkSize;
    uint32_t mPreviousChunkCount;
};


struct FlexiblePool
{
    template<typename T>
    using Allocator = Detail::Allocator<FlexiblePool, T>;

    FlexiblePool()
    {
    }

    FlexiblePool(const FlexiblePool&) = delete;
    FlexiblePool& operator=(const FlexiblePool&) = delete;

    FlexiblePool(FlexiblePool&&) = default;
    FlexiblePool& operator=(FlexiblePool&&) = default;

    ~FlexiblePool()
    {
    }

    void* allocate(std::size_t block_size)
    {
        //// LOG(Critical) << "Allocate " << n;
        if (block_size <= 256)
        {
            SmallObjectPool& pool = mFixedChunkingPools[block_size];
            return pool.allocate(block_size);
        }
        else
        {
            SimplePool& pool = mFixedFreeListPools[block_size];
            return pool.allocate(block_size);
        }
    }

    void deallocate(void* data, std::size_t block_size)
    {
        //// LOG(Critical) << "Deallocate " << n;

        if (block_size <= 256)
        {
            SmallObjectPool& pool = mFixedChunkingPools[block_size];
            pool.deallocate(data, block_size);
        }
        else
        {
            SimplePool& pool = mFixedFreeListPools[block_size];
            return pool.deallocate(data, block_size);
        }
    }

    boost::container::flat_map<int, SimplePool> mFixedFreeListPools;
    boost::container::flat_map<int, SmallObjectPool> mFixedChunkingPools;
};



template<typename T, typename Base = std::vector<T, FlexiblePool::Allocator<T>>>
using Vector = std::vector<T, FlexiblePool::Allocator<T>>;

using String = std::basic_string<char, std::char_traits<char>, FlexiblePool::Allocator<char>>;



template<typename Alloc>
void test(Alloc&& alloc)
{

    Vector<String> vec{alloc};
    vec.resize(1);
    vec.back() = "abcabcabcabcabcabcabcabcabcabcabcabcabcabcabc";
    vec.push_back(String("defdefdefdefdefdefdefdefdefdefdefdefdefdefdefdefdefdefdefdefdefdefdef", alloc));
    vec.emplace_back("ghighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighighi");

    auto copy = vec;
    vec.insert(vec.begin(), copy.begin(), copy.end());
}



int main()
{
    {

        FlexiblePool alloc;

        for (int i = 0; i != 10; ++i)
        {
            test(alloc);
        }
    }

    std::cout << "End of program" << std::endl;
}
