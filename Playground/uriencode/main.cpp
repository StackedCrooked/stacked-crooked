#include <iostream>
#include <string>
#include <stdint.h>


int int_bytes[256] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};


void encode(std::ostream& os, char c)
{
    switch (c)
    {
        case '!' : os.write("%21", 3u); return;
        case '#' : os.write("%23", 3u); return;
        case '$' : os.write("%24", 3u); return;
        case '&' : os.write("%26", 3u); return;
        case '\'': os.write("%27", 3u); return;
        case '(' : os.write("%28", 3u); return;
        case ')' : os.write("%29", 3u); return;
        case '*' : os.write("%2A", 3u); return;
        case '+' : os.write("%2B", 3u); return;
        case ',' : os.write("%2C", 3u); return;
        case '/' : os.write("%2F", 3u); return;
        case ':' : os.write("%3A", 3u); return;
        case ';' : os.write("%3B", 3u); return;
        case '=' : os.write("%3D", 3u); return;
        case '?' : os.write("%3F", 3u); return;
        case '@' : os.write("%40", 3u); return;
        case '[' : os.write("%5B", 3u); return;
        case ']' : os.write("%5D", 3u); return;
        default  : os.put(static_cast<char>(int_bytes[uint8_t(int(c))]));
    }
}


void read()
{
    std::string buf;
    while (std::cin >> buf)
    {
        for (char c : buf)
        {
            encode(std::cout , c);
        }
    }
}


void self_test()
{
    std::string result;
    for (int i = 0; i != 255; ++i)
    {
        std::stringstream ss;
        encode(of, char(i));

        char verify;
        ss >> verify;
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << int(verify);
    }
}


int main()
{
    self_test();
    read();
}
