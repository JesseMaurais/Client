#ifndef key_hpp
#define key_hpp "Optimal String Store"

#include "fmt.hpp"
#include "opt.hpp"
#include "tmp.hpp"

namespace env::opt
{
	using fmt::string;
	using string::view;

	using vector = fmt::vector<word>;
	using span = fmt::span<word>;
	using init = fmt::init<word>;

	bool got(word); // whether word is stored
	bool got(view); // whether string is cached
	view get(word); // view for a word
	view get(view); // unique in view cache
	word put(view); // add string view
	word set(view); // new string view

	inline auto all(view::span line, auto to)
	// convert many at once
	{
		vector words;
		for (auto it : line) words.emplace_back(to(it));
		return words;
	}

	inline auto get(auto line)
	// get all words in line
	{
		return all(line, get);
	}

	inline auto put(auto line)
	// put all words in line
	{
		return all(line, put);
	}

	inline auto set(auto line)
	// set all words in line
	{
		return all(line, set);
	}
}

#endif // file
