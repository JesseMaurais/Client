#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "fmt.hpp"

namespace doc
{
	struct ini : fmt::memory<ini>
	{
		fwd::map<fmt::pair, fmt::view> keys;
		fmt::string::set cache;

		friend fmt::input operator>>(fmt::input, ref);
		friend fmt::output operator<<(fmt::output, cref);
		static fmt::input getline(fmt::input, fmt::string::ref);

		bool got(fmt::pair) const;
		fmt::view get(fmt::pair) const;
		bool set(fmt::pair, fmt::view);
		bool put(fmt::pair, fmt::view);
	};
}

#endif // file
