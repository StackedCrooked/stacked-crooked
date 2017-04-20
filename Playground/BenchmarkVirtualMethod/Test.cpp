#include "Test.h"


int Test::foo()
{
    return 1;

}


Test& get_test()
{
    static Test t;
    return t;
}
