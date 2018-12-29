#ifndef ini_hpp
#define ini_hpp

#include "fmt.hpp"
#include <istream>
#include <map>

namespace ini
{
	static std::istream& getline(std::istream& in, std::string& s)
	{
		constexpr auto c = '#';
		while (std::getline(in, s))
		{
			auto const it = skip(s.begin(), s.end());
			if (it != s.end() and c != *it)
			{
				auto const t = s.find(c);
				s = s.substr(0, t);
				if (trim(s))
				{
					break;
				}
			}
		}
		return in;
	}
}

#endif // file
