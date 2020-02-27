#ifndef key_hpp
#define key_hpp

#include "opt.hpp"
#include <vector>

namespace env::opt
{
	using array = std::vector<word>;
	using words = fmt::span<word>;
	using list = fmt::vector_view;

	bool got(word key);
	view get(word key);
	word set(view key);
	word put(view key);

	list get(words say);
	array set(span say);
	array put(span say);
}

#endif // file
