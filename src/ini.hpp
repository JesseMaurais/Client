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
		using string = fmt::string;
		using view = fmt::string_view;
		using pair = fmt::pair_view;
		using span = fmt::span_view;
		using vector = fmt::vector_view;

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
