#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "fmt.hpp"

namespace doc
{
	struct ini : fmt::memory<ini>
	{
		fmt::view::vector values;
		fmt::string::set cache;
		fwd::group<int> keys;

		using pair = fwd::group<int>::key_type;
		using string = fmt::string;
		using view = fmt::view;

		friend view::in::ref operator>>(view::in::ref, ref);
		friend view::out::ref operator<<(view::out::ref, cref);
		static view::in::ref getline(view::in::ref, string::ref);

		bool got(pair) const;
		view get(pair) const;
		bool set(pair, view);
		bool put(pair, view);
	};
}

#endif // file
