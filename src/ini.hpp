#ifndef ini_hpp
#define ini_hpp

#include "str.hpp"
#include <iosfwd>
#include <set>
#include <map>

namespace doc
{
	struct ini
	{
		using fmt::string;
		using string::view;
		using view::pair;
		using view::span;
		using view::vector;

		static string join(span value);
		static vector split(view value);
		static std::istream & getline(std::istream &, string &);
		friend std::istream & operator>>(std::istream &, ini &);
		friend std::ostream & operator<<(std::ostream &, ini &);

		bool got(pair key) const;
		view get(pair key) const;
		bool set(pair key, view value);
		bool put(pair key, view value);
		bool cut(pair key);

	private:

		std::map<pair, view> map;
		std::set<string> buf;
		view store(view);
	};
}

#endif // file
