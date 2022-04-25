#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "doc.hpp"

namespace doc
{
	struct ini : fmt::memory<ini>
	{
		using string = fmt::string;
		using view   = string::view;
		using init   = view::init;
		using vector = view::vector;
		using span   = view::span;
		using out    = string::out;
		using in     = string::in;

		path::group keys;
		string::view::vector values;
		string::set cache;

		friend in::ref operator>>(in::ref, ref);
		friend out::ref operator<<(out::ref, cref);
		static in::ref getline(in::ref, string::ref);

		static string join(span);
		static vector split(view);
		static string join(init);

		bool got(path::pair) const;
		view get(path::pair) const;
		bool set(path::pair, view);
		bool put(path::pair, view);
	};
}

#endif // file
