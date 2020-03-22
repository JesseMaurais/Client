#ifndef key_hpp
#define key_hpp

// local headers
#include "fmt.hpp"
#include "opt.hpp"

namespace env::opt
{
	using vector = fmt::vector<word>;
	using span = fmt::span<word>;
	using view = fmt::string::view;

	bool got(word key); /// Whether $key exists
	view get(word key); /// Value at $key
	word set(view key); /// Index for value of $key
	word put(view key); /// Index for $key

	view::vector get(view::span say);
	vector set(span say);
	vector put(span say);

	inline view dup(view::ref u)
	{
		return u = get(set(u));
	}
}

#endif // file
