#ifndef win_profile_hpp
#define win_profile_hpp "WIN32 Profile"

#include "win.hpp"
#include "ptr.hpp"

namespace sys::win
{
    using performance = fwd::zero<LARGE_INTEGER>;

    struct counter : performance
    {
        counter()
        {
            if (not QueryPerformanceCounter(this))
            {
                sys::win::err(here, "QueryPerformanceCounter");
            }
        }
    };

    struct frequency : performance
    {
        frequency()
        {
            if (not QueryPerformanceFrequency(this))
            {
                sys::win::err(here, "QueryPerformanceFrequency");
            }
        }
    };
}

#endif // file