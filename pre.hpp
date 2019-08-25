// Precompiled Header

// C

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <csignal>
#include <csetjmp>
#include <cstdlib>
#include <cstdio>
#include <cuchar>
#include <cwchar>
#include <cerrno>
#include <cmath>
#include <ctime>

// C++

#include <initializer_list>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include <streambuf>
#include <charconv>
#include <locale>
#include <regex>
#include <string>
#include <iterator>
#include <vector>
#include <array>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <memory>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <limits>

// OS

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <tlhelp32.h>
# include <winsock2.h>
# include <shlobj.h>
# include <process.h>
# include <direct.h>
# include <io.h>
#else
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <sys/wait.h>
# include <sys/un.h>
# include <poll.h>
# include <sys/mman.h>
# include <pthread.h>
# include <setjmp.h>
# include <dirent.h>
# include <dlfcn.h>
# include <unistd.h>
#endif

