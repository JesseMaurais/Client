#ifndef ini_hpp
#define ini_hpp

#include "fmt.hpp"
#include <istream>
#include <string>
#include <map>

namespace ini
{
	using entry = std::map<std::string, std::string>;
	using group = std::map<std::string, entry>;

	static std::istream& getline(std::istream& in, std::string& s, char const c = '#')
	{
		while (std::getline(in, s))
		{
			auto const it = fmt::next(s.begin(), s.end());
			if (it != s.end() and c != *it)
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
