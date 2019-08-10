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
	using pair = fmt::string_pair;
	using hash = std::map<pair, string>;

	istream & getline(istream &, string &, char c = '#');
	bool header(view);

	struct keys
	{
		keys(istream &);
		view get(pair key) const;
		bool set(pair key, view value);

	protected:

		hash map;
	};
}

#endif // file
