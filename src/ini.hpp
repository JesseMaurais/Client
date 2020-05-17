#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "fmt.hpp"
#include "opt.hpp"
#include "algo.hpp"

namespace doc
{
	struct ini : fmt::struct_brief<ini>
	{
		using string = fmt::string;
		using in     = string::in;
		using out    = string::out;
		using view   = string::view;
		using pair   = view::pair;
		using span   = view::span;
		
		fwd::group<env::opt::word> keys;
		string::view::vector values;
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
