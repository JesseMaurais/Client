#ifndef str_hpp
#define str_hpp "Shared String Store"

#include "fmt.hpp"
#include "opt.hpp"

namespace fmt::str
{
	using string = fmt::string;
	using view = string::view;
	using span = view::span;
	using word = env::opt::word;
	using vector = env::opt::vector;

	bool got(word); // O (1)
	bool got(view); // O log n
	view get(word); // O (1)
	view get(view); // O log n
	word put(view); // O log n
	word set(view); // O log n

	string::in::ref get(string::in::ref, char = eol);
	// read all file lines to cache
	string::out::ref put(string::out::ref, char = eol);
	// write all cache lines to file

	inline auto put(span list)
	// put all words in list
	{
		vector words;
		for (auto item : list) words.emplace_back(put(item));
		return words;
	}

	inline auto set(span list)
	// set all words in list
	{
		vector words;
		for (auto item : list) words.emplace_back(set(item));
		return words;
	}
}

#endif // file
