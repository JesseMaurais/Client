#ifndef set_hpp
#define set_hpp

#include <algorithm>
#include <vector>

template <class Object, auto Counter = short> struct ordered_set
{
	std::vector<Object> set;
	std::vector<Counter> index;

	auto const & at(auto n) const
	{
		return set.at(index.at(n));
	}

	auto & at(auto n)
	{
		return set.at(index.at(n));
	}

	auto emplace(Object && obj)
	{
		auto it = std::find(index.begin(), index.end(), -1);
		if (index.end() == it)
		{
			index.push_back(-1);
			it = index.end() - 1;
		}
		auto const n = static_cast<Counter>(size(set));
		#ifdef assert
		set.emplace_back(obj);
	}
};

#endif // file
