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

	bool got(word key);
	view get(word key);
	word set(view key);
	word put(view key);

	view::vector get(view::span say);
	vector set(span say);
	vector put(span say);
}

#endif // file
