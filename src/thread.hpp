#ifndef thread_hpp
#define thread_hpp

#ifdef _WIN32
# include "win/thread.hpp"
#else
# include "uni/pthread.hpp"
#endif

#endif // file
