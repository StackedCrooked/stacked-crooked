#include <iostream>


template<typename Tag> static int const init_get() {
  static int val;
  return val++;
}


#define Futile_BeginEnum(N) \
  namespace N { \
    typedef struct t_ ## enum_ ## N tag_type_;

#define Futile_EndEnum(N) }


#define Futile_EnumEntry(Name) \
  static int const Name = init_get<tag_type_>(); \
  struct Name { static char const *name() { return #Name; } };


Futile_BeginEnum(RGB)
  Futile_EnumEntry(Red)
  Futile_EnumEntry(Green)
  Futile_EnumEntry(Blue)
Futile_EndEnum(RGB)

Futile_BeginEnum(Notes)
  Futile_EnumEntry(Do)
  Futile_EnumEntry(Re)
  Futile_EnumEntry(Mi)
Futile_EndEnum(Notes)


int main()
{
  std::cout << "RGB ids: " << RGB::Red << ", " << RGB::Green << ", " << RGB::Blue << std::endl;
  std::cout << "RGB strings: " << RGB::Red::name() << ", " << RGB::Green::name() << ", " << RGB::Blue::name() << std::endl;

  std::cout << "Notes ids: " << Notes::Do << ", " << Notes::Re << ", " << Notes::Mi << std::endl;
  std::cout << "Notes string: " << Notes::Do::name() << ", " << Notes::Re::name() << ", " << Notes::Mi::name() << std::endl;
}
