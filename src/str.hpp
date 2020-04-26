#ifndef str_hpp
#define str_hpp "Shared String Store"

#include "fmt.hpp"
#include "opt.hpp"

namespace fmt::str
{
	using string::view;
	using view::span;
	using env::opt::word;
	using env::opt::vector;

	bool got(word); // O (1)
	bool got(view); // O log n : n is the cache size
	view get(word); // O (1)
	view get(view); // O log n
	word put(view); // O log n
	word set(view); // O log n

	string::in::ref get(string::in::ref, char = eol);
	// read all file lines to cache
	string::out::ref put(string::out::ref, char = eol);
	// write all cache lines to file

	inline auto all(auto list, auto to)
	// convert many at once
	{
		vector words;
		for (auto it : list) words.emplace_back(to(it));
		return words;
	}

	inline auto get(auto list)
	// get all words in list
	{
		return all(list, get);
	}

	inline auto put(auto list)
	// put all words in list
	{
		return all(list, put);
	}

	inline auto set(auto list)
	// set all words in list
	{
		return all(list, set);
	}
}

#endif // file
