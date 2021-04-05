#ifndef msg_hpp
#define msg_hpp "Document Messages"

#include "doc.hpp"
#include <functional>

namespace doc
{
	using function = std::function<void()>;
	extern template instance<function>;
	using slot = access_ptr<function>;

	inline auto& socket()
	{
		return access<function>();
	}

	inline void signal(size_t id)
	{
		#ifdef assert
		assert(socket().find(id));
		#endif

		std::invoke(socket().at(id));
	}
};

#endif // file
