#ifndef ini_hpp
#define ini_hpp

#include "str.hpp"
#include <istream>
#include <set>
#include <map>

namespace ini
{
	using std::istream;
	using fmt::string;
	using view = fmt::string_view;
	using pair = fmt::string_view_pair;

	istream & getline(istream &, string &, char c = '#');
	bool header(view);

	struct keys
	{
		keys(istream &);
		view get(pair key) const;
		bool set(pair key, view value);
		bool put(pair key, view value);

	private:

		std::map<pair, view> map;
		std::set<string> buf;
		view store(view);
	};
}

#endif // file
