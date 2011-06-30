#include "Reflex/Object.h"
#include "Reflex/Type.h"
#include "Person.h"
#include <iostream>
#include <cassert>


using namespace Reflex;


int main()
{
    Type t = Type::ByName("Person");
    if (!t)
    {
        std::cout << "Failed to load meta data for Person class." << std::endl;
        return 1;
    }

    if (t.IsClass())
    {
        std::cout << "We found a class type" << std::endl;
    }


    if (t.TypeType() == CLASS)
    {
        std::cout << "This is still a class" << std::endl;
    }

    //std::cout << "Allocate object" << std::endl;
    //void * v = t.Allocate();
    //assert(v);

    //std::cout << "Destruct object" << std::endl;
    //t.Destruct(v);

    std::cout << "Construct object" << std::endl;
    Object o = t.Construct();

    for (Member_Iterator it = t.Member_Begin(); it != t.Member_End(); ++it)
    {
        switch ((*it).MemberType())
        {
            case DATAMEMBER:
            {
                std::cout << "Data member: " << (*it).Name() << " at offset " << (*it).Offset() << "." << std::endl;
                break;
            }
            case FUNCTIONMEMBER:
            {
                std::cout << "Function member: " << (*it).Name() << " has " << (*it).FunctionParameterSize() << " parameters." << std::endl;
                break;
            }
        }
    }

    return 0;
}

