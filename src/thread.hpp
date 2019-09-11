#ifndef thread_hpp
#define thread_hpp

#ifdef _WIN32
# include "win/sync.hpp"
#else
# include "uni/pthread.hpp"
#endif

#endif // file
