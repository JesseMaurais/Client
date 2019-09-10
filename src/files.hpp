#ifndef files_hpp
#define files_hpp

#ifdef _WIN32
# include "win/file.hpp"
#else
# include "uni/dirent.hpp"
#endif

#endif // file
