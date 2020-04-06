#ifndef key_hpp
#define key_hpp "Option String Keys"

// local headers
#include "fmt.hpp"
#include "opt.hpp"

namespace env::opt
{
	using fmt::string;
	using string::view;
	using view::span;

	using vector = fmt::vector<word>;

	bool got(word); // whether word is a string
	bool got(view); // whether string exists
	view get(word); // string for a word
	word get(view); // word for a string
	word put(view); // add string view
	word set(view); // add string copy

	inline auto dup(view::ref u)
	// duplicate and return new
	{
		return u = get(set(u));
	}

	inline auto all(span say, auto to)
	// convert many at once
	{
		vector items;
		for (auto it : say) items.emplace_back(to(it));
		return items;
	}

	inline auto gets(span say)
	// get words in sentence
	{
		return all(say, get);
	}

	inline auto puts(span say)
	// put words in sentence
	{
		return all(say, put);
	}

	inline auto sets(span say)
	// set words in sentence
	{
		return all(say, set);
	}
}

#endif // file
