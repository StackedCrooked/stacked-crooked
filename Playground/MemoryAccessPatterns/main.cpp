#include <iostream>
#include <vector>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

volatile const auto volatile_zero = 0ul; // disables some optimizations

const auto num_iterations = NUM_ITERATIONS + volatile_zero;
const auto array_size = ARRAY_SIZE + volatile_zero;


std::vector<long> array1(array_size);
std::vector<long> array2(array_size);
std::vector<long> array3(array_size);
std::vector<long> array4(array_size);
std::vector<long> array5(array_size);
std::vector<long> array6(array_size);
std::vector<long> array7(array_size);
std::vector<long> array8(array_size);


int main()
{
    
    auto result = 0ul;

    auto start_time = Clock::now();

    for (auto it = 0ul; it != num_iterations; ++it)
    {
        for (auto i = 0ul; i != array_size; ++i)
        {
            result += array1[i]++;
            result += array2[i]++;
            result += array3[i]++;
            result += array4[i]++;
            result += array5[i]++;
            result += array6[i]++;
            result += array7[i]++;
            result += array8[i]++;
        }
    }
    
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start_time).count();
    
    std::cout << "Iterate " << num_iterations << " times over 8x" << (array_size * sizeof(array1[0])) << "B => " << elapsed_ms << "ms\n";

    return result;
}
//