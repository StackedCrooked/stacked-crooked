#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED


#include <string>


namespace HSServer
{

    enum Location
    {
        Location_Unknown,
        Location_Begin,
        Location_hs = Location_Begin,
        Location_hs_add,
        Location_hof,
        Location_End
    };

    template<Location inLocation>
    struct GetLocation_t
    {
    };

    template<>
    struct GetLocation_t<Location_hs>
    {
        static const char * GetValue() { return "/hs"; }
    };

    template<>
    struct GetLocation_t<Location_hs_add>
    {
        static const char * GetValue() { return "/hs/add"; }
    };

    template<>
    struct GetLocation_t<Location_hof>
    {
        static const char * GetValue() { return "/hof"; }
    };

} // namespace HSServer


#endif // LOCATION_H_INCLUDED
