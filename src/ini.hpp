#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "it.hpp"
#include "fmt.hpp"
#include <iosfwd>

namespace doc
{
	struct ini : fmt::struct_traits<ini>
	{
		using fmt::string;
		using string::view;
		using string::in;
		using string::out;
		using view::pair;
		using view::span;
		using view::init;
		using view::vector;
		
		fwd::group<view> keys;
		fwd::edges<view> values;
		fwd::vector<string> cache;

		static in::ref getline(in::ref, string::ref);
		friend in::ref operator>>(in::ref, ini::ref);
		friend out::ref operator<<(out::ref, ini::cref);

		static string join(span);
		static vector split(view);
		static string join(init);

		bool got(pair) const;
		view get(pair) const;
		bool set(pair, view);
		bool put(pair, view);
	};
}

#endif // file
