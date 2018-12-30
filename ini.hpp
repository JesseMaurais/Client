#ifndef ini_hpp
#define ini_hpp

#include "fmt.hpp"
#include <istream>
#include <string>
#include <map>

namespace ini
{
	using map = std::map<std::string, std::string>;

	static std::istream& getline(std::istream& in, std::string& s)
	{
		constexpr auto c = '#';
		while (std::getline(in, s))
		{
			auto const it = fmt::skip(s.begin(), s.end());
			if (it != s.end() and c != *it)
			{
				auto const t = s.find(c);
				s = s.substr(0, t);
				if (fmt::trim(s))
				{
					break;
				}
			}
		}
		return in;
	}
}

#endif // file
