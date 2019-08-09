#ifndef ini_hpp
#define ini_hpp

#include "str.hpp"
#include <istream>
#include <map>
#include <set>

namespace ini
{
	using std::istream;
	using fmt::string;
	using fmt::string_view;
	using fmt::string_view_pair;
	using value = string_view;
	using cache = std::set<string>;
	using group = std::map<value, value>;
	using entry = group::value_type;
	using order = std::map<value, group>;

	istream& getline(istream& in, string& s, char c = '#');
	bool header(string_view u);

	struct keys
	{
		keys(string_view path);
		value get(entry) const;
		void set(entry, value);
		void put(entry, value);

	protected:

		order tree;
		cache buf;

	private:

		value store(value);
	};
}

#endif // file
