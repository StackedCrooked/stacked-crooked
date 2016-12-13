#include <cstdint>
#include <numeric>
#include <vector>
#include <thread>
#include <iostream>


using Integer = uint64_t;


struct Matrix
{
    Matrix(int w, int h) :
        mWidth(w),
        mHeight(h),
        mVec(w * h)
    {
    }

    Integer get(int x, int y) const
    {
        return *(mVec.data() + y * mWidth + x);
    }

    void set(int x, int y, Integer n)
    {
        *(mVec.data() + y * mWidth + x) = n;
    }

    Integer sum_slow() const
    {
        auto result = Integer();
        for (auto x = 0u; x != mWidth; ++x)
        {
            for (auto y = 0u; y != mHeight; ++y)
            {
                result += get(x, y);
            }
        }
        return result;
    }

    Integer sum_fast() const
    {
        auto result = Integer();

        for (auto y = 0u; y != mHeight; ++y)
        {
            for (auto x = 0u; x != mWidth; ++x)
            {
                result += get(x, y);
            }
        }
        return result;
    }

    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<uint64_t> mVec;

};


using Clock = std::chrono::steady_clock;
volatile Integer volatile_sink;
volatile const Integer volatile_zero = 0;


template<typename F>
int64_t benchmark(F&& f)
{
    using Clock = std::chrono::steady_clock;
    auto start_time = Clock::now();
    volatile_sink = f();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start_time).count();
}


int main()
{
    Matrix m(64 * 1024, 512);

    for (auto i = 0; i != 10; ++i)
    {
        auto t1 = benchmark([&] { return m.sum_fast(); });
        auto t2 = benchmark([&] { return m.sum_slow(); });
        std::cout << t1 << "/" << t2 << "=" << double(t1)/t2 << std::endl;
    }
}
