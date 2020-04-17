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

	bool got(word); // O (1)
	bool got(view); // O log n : n is the cache size
	view get(word); // O (1)
	view get(view); // O log n
	word put(view); // O log n
	word set(view); // O log n

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
