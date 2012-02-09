#include <cstring>
#include <stdint.h>
#include <iostream>


static void * get_data()
{
    static char data[6] = { 1, 2, 3, 4, 5, 6 };
    return data;
}

struct POD
{
    uint16_t a;
    uint16_t b;
};


POD get_from_data(void * data)
{
    POD result;
    memcpy(&result, data, sizeof(result));
    return result;
}

bool equals(void * a, void * b)
{
    return (*(uint16_t*)a) == (*(uint16_t*)b) && (*(uint32_t*)a) == (*(uint32_t*)b);
}

int main()
{
    std::cout << "Break aliasing: " << (*reinterpret_cast<const POD *>(get_data())).a << std::endl;
    std::cout << "No break aliasing: " << get_from_data(get_data()).a << std::endl;
    std::cout << "Try break aliasing: " << equals(get_data(), get_data()) << std::endl;
    return 0;
}
