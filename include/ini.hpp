#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "fmt.hpp"

namespace doc
{
	struct ini : fmt::memory<ini>
	{
		fmt::view::vector values;
		fmt::string::set cache;
		fmt::view::group keys;

		using string = fmt::string;
		using view = fmt::view;
		using pair = fmt::pair;

		friend fmt::input operator>>(fmt::input, ref);
		friend fmt::output operator<<(fmt::output, cref);
		static fmt::input getline(fmt::input, string::ref);

		bool got(pair) const;
		view get(pair) const;
		bool set(pair, view);
		bool put(pair, view);
	};
}

#endif // file
