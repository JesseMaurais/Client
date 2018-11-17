#ifndef ev_hpp
#define ev_hpp

#include <sig.hpp>

namespace sys::ev
{
	class poll : public ::sig::event<int, int>
	{
		bool wait(int timeout = -1) const;
	};
};

#endif // file