#include <stdio.h>


const unsigned long long size = 8ULL*1024ULL*1024ULL;
unsigned long long a[size];

int main()
{
    FILE* pFile = fopen("file.binary", "wb");

    for (unsigned long long j = 0; j < 1024; ++j)
    {
        //Some calculations to fill a[]
        fwrite(a, 1, size*sizeof(unsigned long long), pFile);
    }
    fclose(pFile);
    return 0;
}
