#ifndef msg_hpp
#define msg_hpp "Document Messages"

#include "doc.hpp"
#include <functional>

namespace doc
{
	using function = std::function<void()>;
	extern template class instance<function>;

	inline size_t socket(function f)
	{
		#ifdef assert
		assert(f.target() != nullptr);
		#endif

		return access<function>().emplace(f);
	}

	inline size_t unsocket(size_t id)
	{
		return access<function>().free(id);
	}

	inline void signal(size_t id)
	{
		#ifdef assert
		assert(access<function>().find(id));
		#endif

		std::invoke(access<function>().at(id));
	}
};

#endif // file
