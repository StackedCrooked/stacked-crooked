#include <cstring>
#include <iostream>
#include <iomanip>
#include <random>
#include <type_traits>
#include <vector>
#include <sys/time.h>


#define BAD_CODE  0
#define GOOD_CODE 1


#if 0


const uint32_t * csrc = reinterpret_cast<const uint32_t *>(this + sizeof(RTPHeader));




#endif


void example_checksum16()
{


}

/**
 * buggy_decode tries to create a c++ object from a network data packet.
 *
 * This implementation is buggy for the following reasons:
 * (1) According to the c++ type system no object exists in that memory
 *     location. We are not allowed to alias a non-existing object. (*)
 * (2) The pointer address of the network data may not match the native
 *     aligment of the target type. Misaligned objects can lead to
 *     performance overhead (Intel) or to process termination (ARM).
 *
 * (*) Technically we could use placement-new to construct an object that
 *     overlays the memory segment. However, that doesn't help us much
 *     futher because it does nothing to prevent the potential alignment
 *     problems. Furthermore the constructed object must be initialized
 *     before we can legally read from its contents. "Initialization" implies
 *     overwriting the network packet data. This obviously defeats our purpose.
 */
template<typename T>
inline const T & buggy_decode(const uint8_t * bytes)
{
    static_assert(std::is_pod<T>::value, "T must meet POD requirements.");
    return *reinterpret_cast<const T *>(bytes);
}

/**
 * correct_decode creates a c++ pod object from a network data packet.
 *
 * First we ensure that our c++ has native alignment by allocating it
 * at a local variable on the stack.
 * Next we obtain a byte pointer to the memory segment occupied by our
 * object. This form of aliasing is explicitly allowed by the c++ standard.
 *
 * The byte pointer can be obtained in two ways:
 *   (1) reinterpret_cast<uint8_t*>(&t);
 *   (2) static_cast<uint8_t*>(static_cast<void*>(&t));
 *
 * We must return the object by value because it is a local variable. In theory
 * this means an extra copy. In practice this copy is fully optimized away by
 * thanks to the Named-Return-Value-Optimization.
 */
template<typename T>
T correct_decode(const uint8_t * bytes)
{
    T t;
    std::copy(bytes, bytes + sizeof(T), static_cast<uint8_t*>(static_cast<void*>(&t)));
    return t;
}


//
// Benchmark
//
struct NetworkData
{
    uint8_t  dst_mac[6];
    uint8_t  src_nac[6];
    uint16_t ethertype;
};


uint8_t GetRandomByte()
{
    static auto rng = []() -> std::mt19937 {
        std::mt19937 rng;
        rng.seed(time(0));
        return rng;
    }();
    static std::uniform_int_distribution<uint8_t> dist(0, 3);
    return dist(rng);
}


double get_current_time()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return double (tv.tv_sec) + 0.000001 * tv.tv_usec;
}


std::vector<uint8_t> get_random_buffer(unsigned count)
{
    std::vector<uint8_t> result(sizeof(NetworkData) * count);

    for (uint8_t & c : result)
    {
        c = GetRandomByte();
    }
    return result;
}


struct buggy_decoder
{
    const NetworkData & operator()(const uint8_t * data) const
    {
        static_assert(std::is_pod<NetworkData>::value, "T must meet POD requirements.");
        return *reinterpret_cast<const NetworkData *>(data);
    }
};


struct correct_decoder
{
    NetworkData operator()(const uint8_t * data) const
    {
        NetworkData hdr;
        uint8_t* dst = reinterpret_cast<uint8_t*>(&hdr);
        std::copy(data, data + sizeof(hdr), dst);
        return hdr;
    }
};

const uint64_t ns = uint64_t(1000) * uint64_t(1000) * uint64_t(1000);


template<typename decode_function_t>
double benchmark_decode(unsigned count, bool force_misalignment, unsigned & optimization_preventer, const decode_function_t & decode_function)
{
    auto buffer = get_random_buffer(count + (force_misalignment ? 1 : 0));

    // this is a "bad" docode! it is only here for the purpose of hopefully finding a good-enough reason against using it



    auto t = get_current_time();
    for (unsigned i = 0; i != count; ++i)
    {
        optimization_preventer += decode_function(buffer.data() + (count * sizeof(NetworkData)) + (force_misalignment ? 1 : 0)).ethertype;
    }
    return get_current_time() - t;
}

template<typename decoder_function_t>
void test_decoder_impl(unsigned count, bool force_misalign, const decoder_function_t & dec, unsigned & optimization_preventer)
{
    double t = benchmark_decode(count, force_misalign, optimization_preventer, dec);
    std::cout << "Cost: " << std::dec << std::setprecision(10) << (ns * t / count) << " ns/iter" << std::endl;
}

void test_decoder()
{
    unsigned optimization_preventer = 0;
    unsigned count = 100000;

    test_decoder_impl(count, false, buggy_decoder(),   optimization_preventer);
    test_decoder_impl(count, true,  buggy_decoder(),   optimization_preventer);
    test_decoder_impl(count, false, correct_decoder(), optimization_preventer);
    test_decoder_impl(count, true,  correct_decoder(), optimization_preventer);

}

int main()
{
    test_decoder();
}
