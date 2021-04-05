#ifndef msg_hpp
#define msg_hpp "Document Messages"

#include "doc.hpp"
#include <functional>

namespace doc
{
	using socket = std::function<void()>;
	extern template instance<socket>;
	using slot = access_ptr<socket>;

	static auto& message()
	{
		return access<socket>();
	}

	static void signal(size_t id)
	{
		#ifdef assert
		assert(socket().find(id));
		#endif

		std::invoke(socket().at(id));
	}
};

#endif // file
