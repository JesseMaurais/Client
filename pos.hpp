#ifndef pos_hpp
#define pos_hpp
// POSIX unless Win32
#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif
#include <queue>
namespace sys
{
	template <typename Routine>
	struct queue : std::queue<sys::thread<Routine>>
	{
		using base = typename std::queue<sys::thread<Routine>>;

		void push(Routine next)
		{
			base::emplace(next);
		}

		~queue()
		{
			base::top.join();
		}
	};
}
#endif
