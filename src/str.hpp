#ifndef str_hpp
#define str_hpp "Shared String Store"

#include "fmt.hpp"

namespace fmt::str
{
	using string::view;
	using word = unsigned short;
	using vector = fwd::vector<word>;
	using span = fwd::span<word>;
	using init = fwd::init<word>;

	bool got(word); // whether word names a string
	bool got(view); // whether string is stored
	view get(word); // view string for a word
	view get(view); // unique view in store
	word put(view); // add this view to store
	word set(view); // cache string to store

	string::in::ref get(string::in::ref);
	// read all file lines to cache
	string::out::ref put(string::out::ref);
	// write all cache lines to file

	inline auto all(view::span line, auto to)
	// convert many at once
	{
		vector words;
		for (auto it : line) words.emplace_back(put(it));
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
