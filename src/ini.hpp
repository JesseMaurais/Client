#ifndef ini_hpp
#define ini_hpp

#include "fmt.hpp"
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
		using string::in;
		using string::out;

		static string join(span value);
		static vector split(view value);
		static in::ref getline(in::ref, string &);
		friend in::ref operator>>(in::ref, ini &);
		friend out::ref operator<<(out::ref, ini &);

		bool got(pair key) const;
		view get(pair key) const;
		bool set(pair key, view value);
		bool put(pair key, view value);
		bool cut(pair key);

	private:

		std::map<pair, view> map;
		string::set buf;
		view store(view);
	};
}

#endif // file
