#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "doc.hpp"

namespace doc
{
	struct ini : fmt::struct_brief<ini>
	{
		using string = fmt::string;
		using view   = string::view;
		using init   = view::init;
		using vector = view::vector;
		using span   = view::span;
		using out    = string::out;
		using in     = string::in;

		string::view::vector values;
		string::set cache;
		doc::group keys;

		friend in::ref operator>>(in::ref, ref);
		friend out::ref operator<<(out::ref, cref);
		static in::ref getline(in::ref, string::ref);

		static string join(span);
		static vector split(view);
		static string join(init);

		bool got(doc::pair) const;
		view get(doc::pair) const;
		bool set(doc::pair, view);
		bool put(doc::pair, view);
	};
}

#endif // file
