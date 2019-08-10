#ifndef ini_hpp
#define ini_hpp

#include "str.hpp"
#include <istream>
#include <vector>
#include <set>

namespace ini
{
	using std::istream;
	using fmt::string;
	using view = fmt::string_view;
	using cache = std::vector<string>;
	using group = std::map<view, view>;
	using hash = std::map<view, group>;
	using entry = group::value_type;

	istream & getline(istream &, string &, char c = '#');
	bool header(view);

	struct keys
	{
		keys(istream &);
		view get(entry) const;
		void set(entry, view);
		void put(entry, view);

	protected:

		hash tree;
		cache buf;
	};
}

#endif // file
