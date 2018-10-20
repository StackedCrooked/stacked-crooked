#include <array>
#include <cassert>
#include <memory>
#include <vector>
#include <new>
#include <bitset>
#include <iosfwd>

#include <iostream>


inline int log2(int x)
{
    int y;
    asm( "\tbsr %1, %0\n"
        : "=r"(y)
        : "r" (x));
    return y;
}


inline int nextpow2(int x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}


struct BitsetBlock
{
    static_assert(sizeof(unsigned long) == 8, "");

    BitsetBlock() : mBlock() { }

    void set_all() { mBlock = -1; }

    void set(std::size_t i)
    {
        mBlock |= (1ul << i);
    }

    void unset(std::size_t i)
    {
        mBlock &= ~(1ul << i);
    }

    bool get(std::size_t i) const
    {
        return mBlock & (1ul << i);
    }

    std::size_t allocate()
    {
        std::size_t index = __builtin_ctzl(mBlock);
        assert(get(index));
        unset(index);
        assert(!get(index));
        return index;
    }

    void deallocate(std::size_t i)
    {
        assert(!get(i));
        set(i);
    }

    static std::size_t size()
    {
        return 8 * sizeof(BitsetBlock);
    }

    std::size_t available() const
    {
        return __builtin_popcountl(mBlock);
    }

    friend std::ostream& operator<<(std::ostream& os, BitsetBlock block);

    unsigned long mBlock;
};


template<int N>
struct Bitset
{
    static_assert(N % 64 == 0, "");

    enum
    {
        num_blocks = N / 64
    };

    BitsetBlock& get_block(std::size_t block_index)
    {
        assert(block_index < mBlocks.size());
        return mBlocks[block_index];
    }


    void set(std::size_t i)
    {
        get_block(i / 64).set(i % 64);
    }

    void set_all()
    {
        for (auto& block : mBlocks)
        {
            block.set_all();
        }
    }

    void unset(std::size_t i)
    {
        get_block(i / 64).unset(i % 64);
    }

    bool get(std::size_t i)
    {
        return get_block(i / 64).get(i % 64);
    }

    std::size_t allocate()
    {
        std::size_t result = 0;
        for (BitsetBlock& block : mBlocks)
        {
            if (block.mBlock != 0)
            {
                return result + block.allocate();
            }
            result += 64;
        }
        return result;
    }

    std::size_t available() const
    {
        auto result = 0ul;
        for (auto& block : mBlocks)
        {
            result += block.available();
        }
        return result;
    }

    static std::size_t size()
    {
        return num_blocks * BitsetBlock::size();
    }

    void deallocate(std::size_t i)
    {
        //std::cout << "Bitset<" << N << ">::deallocate: " << i << std::endl;
        get_block(i / 64).deallocate(i % 64);
    }

    friend std::ostream& operator<<(std::ostream& os, Bitset block)
    {
        return block.print(os);
    }

    std::ostream& print(std::ostream& os) const;

    std::array<BitsetBlock, num_blocks> mBlocks;
};



template<int BlockSize, int BlockCount>
struct Pool
{
    Pool()
    {
        mBitset.set_all();
    }

    char* get(std::size_t index)
    {
        return mBlocks.data() + index * BlockSize;
    }

    char* data_begin()
    {
        return mBlocks.data();
    }

    char* data_end()
    {
        return mBlocks.data() + mBlocks.size();
    }

    void* allocate()
    {
        auto index = mBitset.allocate();
        if (index == BlockCount) return nullptr;
        return get(index);
    }

    void deallocate(void* ptr)
    {
        auto index = static_cast<char*>(ptr) - mBlocks.data();
        assert(index % BlockSize == 0);
        mBitset.deallocate(index / BlockSize);
    }

    Bitset<BlockCount> mBitset;
    std::array<char, BlockCount * BlockSize> mBlocks;
};



template<int BlockSize, int MaxBlockSize, int BlockCount>
struct BuddyPool;


template<int BlockSize>
struct BuddyPool<BlockSize, BlockSize, 64>
{
    enum { BlockCount = 64 };

    char* data_begin()
    {
        return mPool.data_begin();
    }

    char* data_end()
    {
        return mPool.data_end();
    }

    char* get(std::size_t index)
    {
        return mPool.get(index);
    }

    void* do_allocate(std::size_t n)
    {
        assert(n <= BlockSize);
        return mPool.allocate();
    }

    void deallocate(void* ptr, std::size_t)
    {
        mPool.deallocate(ptr);

    }

    std::ostream& print(std::ostream& os) const
    {
        os << "BlockSize(" << BlockSize << ") x " << BlockCount << ": ";
        return mPool.mBitset.print(os) << std::endl;
    }

    Pool<BlockSize, BlockCount> mPool;
};


template<int BlockSize, int MaxBlockSize, int BlockCount>
struct BuddyPool : BuddyPool<BlockSize * 2, MaxBlockSize, BlockCount / 2>
{
    static_assert(BlockCount >= 64, "");
    static_assert(BlockSize < MaxBlockSize, "");
    using Parent = BuddyPool<BlockSize * 2, MaxBlockSize, BlockCount / 2>;

    char* get(std::size_t index)
    {
        auto result = Parent::get(index / 2);
        if (index % 2 != 0)
        {
            result += BlockSize;
        }
        return result;
    }

    void* allocate(std::size_t n)
    {
        std::cout << "Allocate " << n << std::endl;
        if (n > MaxBlockSize)
        {
            std::cout << "Fallback operator new " << n << std::endl;
            
            return ::operator new(n);
        }

        return do_allocate(n < 16 ? 16 : nextpow2(n));
    }

    void* do_allocate(std::size_t n)
    {

        assert(n >= BlockSize);
        if (n > BlockSize)
        {
            return Parent::do_allocate(n);
        }

        if (n == BlockSize)
        {
            if (mBitset.available())
            {
                return get(mBitset.allocate());
            }
            else
            {
                auto result = Parent::do_allocate(2 * n);
                auto ptr_offset = static_cast<char*>(result) - this->data_begin();
                assert(ptr_offset % BlockSize == 0);
                mBitset.set(ptr_offset / BlockSize + 1);
                return result;
            }
        }

        return Parent::do_allocate(2 * n);
    }

    void deallocate(void* ptr, std::size_t n)
    {

        auto cptr = static_cast<char*>(ptr);

        if (cptr >= this->data_begin() && cptr < this->data_end())
        {
            n = n < 16 ? 16 : nextpow2(n);
            if (n == BlockSize)
            {
                std::cout << "Real Deallocate " << n << std::endl;
            }
            if (n > BlockSize)
            {
                Parent::deallocate(ptr, n);
                return;
            }
            auto ptr_offset = cptr - this->data_begin();
            assert(ptr_offset % BlockSize == 0);
            assert(!mBitset.get(ptr_offset / BlockSize + ptr_offset % BlockSize));

            auto index = ptr_offset / BlockSize;
            if (index % 2 == 0)
            {
                if (mBitset.get(index + 1))
                {
                    Parent::deallocate(cptr, 2 * BlockSize);
                    mBitset.unset(index + 1);
                    return;
                }
            }
            else
            {
                if (mBitset.get(index - 1))
                {
                    Parent::deallocate(cptr - BlockSize, 2 * BlockSize);
                    mBitset.unset(index - 1);
                    return;
                }
            }
            mBitset.deallocate(ptr_offset / BlockSize + ptr_offset % BlockSize);
        }
        else
        {
            std::cout << "Fallback operator delete " << n << std::endl;
            ::operator delete(ptr);
        }
    }

    std::ostream& print(std::ostream& os) const
    {
        os << "BlockSize(" << BlockSize << ") x " << BlockCount << ": " << mBitset << "\n";
        return Parent::print(os);
    }

    Bitset<BlockCount> mBitset;



};



#include <algorithm>
#include <vector>




std::ostream& operator<<(std::ostream& os, BitsetBlock block)
{
    for (auto i = 0; i != 64; ++i)
    {
        os << int(block.get(i));
    }
    return os;
}


template<int BlockSize, int MaxBlockSize, int BlockCount>
std::ostream& operator<<(std::ostream& os, const BuddyPool<BlockSize, MaxBlockSize, BlockCount>& bp)
    {
        return bp.print(os);
    }


template<int N>
std::ostream& Bitset<N>::print(std::ostream& os) const
{
    os << mBlocks.front();
    return os;
}

int main()
{
    // test components
    #if 0
    {
        BitsetBlock bsb;
        assert(bsb.available() == 0);
        bsb.set_all();
        assert(bsb.available() == bsb.size());
        std::cout << bsb << std::endl;
        auto a = bsb.allocate();
        assert(bsb.available() == bsb.size() - 1);
        std::cout << bsb << std::endl;
        auto b = bsb.allocate();
        assert(bsb.available() == bsb.size() - 2);
        std::cout << bsb << std::endl;
        auto c = bsb.allocate();
        assert(bsb.available() == bsb.size() - 3);
        std::cout << bsb << std::endl;
        bsb.deallocate(b);
        assert(bsb.available() == bsb.size() - 2);
        std::cout << bsb << std::endl;
        bsb.deallocate(a);
        assert(bsb.available() == bsb.size() - 1);
        std::cout << bsb << std::endl;
        bsb.deallocate(c);
        assert(bsb.available() == bsb.size());
        std::cout << bsb << std::endl;
    }

    Bitset<256> bs;
    bs.set_all();
    std::vector<std::size_t> bits;
    for (auto i = 0; i != 256; ++i)
    {
        bits.push_back(bs.allocate());
        std::cout << bs << std::endl;
    }

    std::random_shuffle(bits.begin(), bits.end());

    for (auto& i : bits)
    {
        assert(!bs.get(i));
        bs.deallocate(i);
        assert(bs.get(i));
        std::cout << bs << std::endl;
    }


    Pool<16, 128> pool;
    std::vector<void*> blocks;

    for (auto i = 0; i != 128; ++i)
    {
        auto block = pool.allocate();
        assert(block);
        blocks.push_back(block);
    }

    assert(!pool.allocate());

    std::random_shuffle(blocks.begin(), blocks.end());

    for (auto& block : blocks)
    {
        pool.deallocate(block);
    }


    #endif

    {
        std::cout << "TEST BUDDY POOL " << std::endl;
        BuddyPool<16, 4096, 4 * 4096> bp;
        std::cout << "sizeof(bp) = " << sizeof(bp) << " memory=" << sizeof(bp.mPool.mBlocks) << std::endl;
        std::cout << bp << std::endl;
        auto a = bp.allocate(23);

        std::cout << bp << std::endl;

        auto b = bp.allocate(4);assert(a < b);
        std::cout << bp << std::endl;

        auto c = bp.allocate(9);assert(b < c);
        std::cout << bp << std::endl;

        auto d = bp.allocate(15);assert(c < d);
        std::cout << bp << std::endl;


        auto aa = bp.allocate(232);
        std::cout << bp << std::endl;
        auto bb = bp.allocate(333);
        std::cout << bp << std::endl;
        auto cc = bp.allocate(2323);
        std::cout << bp << std::endl;
        auto dd = bp.allocate(23232);
        std::cout << bp << std::endl;
        bp.deallocate(aa, 232);
        std::cout << bp << std::endl;
        bp.deallocate(bb, 333);
        std::cout << bp << std::endl;
        bp.deallocate(cc, 2323);
        std::cout << bp << std::endl;
        bp.deallocate(dd, 23232);
        std::cout << bp << std::endl;
        bp.deallocate(b, 4);
        std::cout << bp << std::endl;
        bp.deallocate(a, 23);
        std::cout << bp << std::endl;
        bp.deallocate(c, 9);
        std::cout << bp << std::endl;
        bp.deallocate(d, 15);
        std::cout << bp << std::endl;
    }
}
