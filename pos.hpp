#ifndef pos_hpp
#define pos_hpp
#else
#error Do not include from another header
#endif

// POSIX unless Win32
#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif


