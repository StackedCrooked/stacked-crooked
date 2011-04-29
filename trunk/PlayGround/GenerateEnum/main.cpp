#include <iostream>
#include <string>


#define Futile_EnumEntry(EntryName, N) \
    static int const EntryName = N;    \
    struct EntryName {                 \
        static std::string ToString()  \
        { return #EntryName; }         \
    } // semi-colon must be typed when calling this macro


namespace RGB
{
    Futile_EnumEntry(Red,   1);
    Futile_EnumEntry(Green, 2);
    Futile_EnumEntry(Blue,  3);
}


int main()
{
    std::cout << RGB::Red << ", " << RGB::Green << ", " << RGB::Blue << std::endl;
    std::cout << RGB::Red::ToString() << ", " << RGB::Green::ToString() << ", " << RGB::Blue::ToString() << std::endl;
    return 0;
}
