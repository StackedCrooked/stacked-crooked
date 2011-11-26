#include <tuple>

template<unsigned I> struct arg{};
struct unused{};

template<class T1 = unused, class T2 = unused, class T3 = unused>
struct param_pack
{
    typedef std::tuple<T1, T2, T3> tuple_type;
    tuple_type values;

    template<unsigned I>
    auto operator[](arg<I>) -> decltype(std::get<I>(values))
    {
        return std::get<I>(values);
    }
};

int main()
{
  param_pack<int, float, double> p;
  auto u1 = p[arg<1>()];
}
