#ifndef ini_hpp
#define ini_hpp

#include "str.hpp"
#include <istream>
#include <ostream>
#include <set>
#include <map>

struct ini
{
	using string = fmt::string;
	using view = fmt::string_view;
	using pair = fmt::string_view_pair;

	static std::istream & getline(std::istream &, string &);
	friend std::istream & operator>>(std::istream &, ini &);
	friend std::ostream & operator<<(std::ostream &, ini &);

	view get(pair key) const;
	bool set(pair key, view value);
	bool put(pair key, view value);
	bool clear(pair key);

private:

	std::map<pair, view> map;
	std::set<string> buf;
	view store(view);
};

#endif // file
