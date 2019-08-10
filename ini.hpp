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
	using group = std::map<string, string>;
	using hash = std::map<string, group>;
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
	};
}

#endif // file
