#ifndef key_hpp
#define key_hpp

#include "opt.hpp"
#include <vector>

namespace env::opt
{
	using array = std::vector<word>;
	using words = fmt::span<word>;

	bool got(word key);
	view get(word key);
	word set(view key);
	word put(view key);

	static list get(words say)
	{
		list items;
		for (view it : say)
		{
			items.emplace_back(get(it))
		}
		return items;
	}

	static auto set(span say)
	{
		array items;
		for (view it : say)
		{
			items.emplace_back(set(it));
		}
		return items;
	}

	static auto put(span say)
	{
		array items;
		for (view it : say)
		{
			items.emplace_back(put(it));
		}
		return items;
	}
}

#endif // file
