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

		fmt::diff::group keys;
		string::view::vector values;
		string::set cache;

		friend in::ref operator>>(in::ref, ref);
		friend out::ref operator<<(out::ref, cref);
		static in::ref getline(in::ref, string::ref);

		static string join(span);
		static vector split(view);
		static string join(init);

		bool got(node::type) const;
		view get(node::type) const;
		bool set(node::type, view);
		bool put(node::type, view);
	};
}

#endif // file
