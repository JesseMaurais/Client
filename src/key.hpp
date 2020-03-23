#ifndef key_hpp
#define key_hpp

// local headers
#include "fmt.hpp"
#include "opt.hpp"

namespace env::opt
{
	using traits = fmt::struct_traits<word>;
	using traits::graph;
	using traits::node;
	using traits::range;
	using fmt::string;
	using string::view;

	bool got(word);
	view get(word);
	word set(view);
	word put(view);

	view::vector get(view::span say);
	vector set(span say);
	vector put(span say);

	inline view dup(view::ref u)
	{
		return u = get(set(u));
	}
}

#endif // file
