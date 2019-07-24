#ifndef ini_hpp
#define ini_hpp

#include "fmt.hpp"
#include <istream>
#include <map>

namespace ini
{
	using entry = std::map<fmt::string, fmt::string>;
	using group = std::map<fmt::string, entry>;

	static std::istream& getline(std::istream& in, fmt::string& s, char const c = '#')
	{
		while (std::getline(in, s))
		{
			auto const it = fmt::skip(begin(s), end(s));
			if (it != end(s) and c != *it)
			{
				auto const t = s.find(c);
				s = s.substr(0, t);
				auto const u = fmt::trim(s);
				if (not empty(u))
				{
					s = fmt::to_string(u);
					break;
				}
			}
		}
		return in;
	}

	inline bool header(fmt::string_view u)
	{
		return not empty(u) and u.front() == '[' and u.back() == ']';
	}
}

#endif // file
