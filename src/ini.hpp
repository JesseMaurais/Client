#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "fmt.hpp"
#include <iosfwd>
#include <set>
#include <map>

namespace doc
{
	struct ini : fmt::struct_traits<ini>
	{
		using fmt::string;
		using string::view;
		using view::pair;
		using view::span;
		using view::init;
		using view::vector;
		using string::in;
		using string::out;
		
		view::matrix keys;
		string::set values;

		static in::ref getline(in::ref, string::ref);
		friend in::ref operator>>(in::ref, ini::ref);
		friend out::ref operator<<(out::ref, ini::cref);

		static string join(span);
		static vector split(view);
		static string join(list);

		bool got(pair) const;
		view get(pair) const;
		bool set(pair, view);
		bool put(pair, view);
		bool cut(pair);
	};
}

#endif // file
