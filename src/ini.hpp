#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "fmt.hpp"

namespace doc
{
	struct ini : fmt::struct_traits<ini>
	{
		using fmt::string;
		using string::in;
		using string::out;
		using string::view;
		using view::pair;
		using view::span;
		using view::init;
		using view::vector;
		
		fwd::group<word> keys;
		view::vector values;
		string::set cache;

		friend in::ref operator>>(in::ref, ref);
		friend out::ref operator<<(out::ref, cref);
		static in::ref getline(in::ref, string::ref);

		static string join(span);
		static vector split(view);
		static string join(init);

		bool got(pair) const;
		view get(pair) const;
		bool set(pair, view);
	};
}

#endif // file
