#ifndef PORT_H
#define PORT_H

struct Port
{
    Port();
    template<typename T>
    T& test();
};


#endif // PORT_H
