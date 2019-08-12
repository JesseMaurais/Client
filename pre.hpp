// Precompiled Header

// C

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <cuchar>
#include <cwchar>
#include <cerrno>

// C++

#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include <streambuf>
#include <locale>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <limits>
#include <stdexcept>

// OS

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
# include <process.h>
# include <direct.h>
# include <io.h>
#else
# include <sys/wait.h>
# include <unistd.h>
#endif

