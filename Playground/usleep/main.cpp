#include <cstdlib>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return 1;
    }
    usleep(strtol(argv[1], NULL, 10));
}
