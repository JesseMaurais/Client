#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "dia.hpp"
#include "fmt.hpp"

namespace doc
{
	struct ini
	{
		fwd::group<word> keys;
		fwd::graph<word> values;
		fmt::string::vector cache;

		using fmt::string;
		using string::in;
		using string::out;
		using string::view;
		using view::span;
		using view::init;
		using view::vector;

		using word = unsigned short;
		using pair = fwd::pair<word>;

		static in::ref getline(in::ref, string::ref);
		// Parse a non-empty, non-comment line

		friend in::ref operator>>(in::ref, ini::ref);
		// Read key/value sections form input (until eof)

		friend out::ref operator<<(out::ref, ini::cref);
		// Write all key/value sections to output

		static string join(span); // join list with separator
		static vector split(view); // split list by separator
		static string join(init); // join list with separator

		bool got(pair) const;
		view get(pair) const;
		bool set(pair, view);
		bool put(pair, view);
	};
}

#endif // file
