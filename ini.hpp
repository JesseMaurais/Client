#ifndef ini_hpp
#define ini_hpp

#include <utility>
#include <vector>
#include <istream>
#include <string_view>
#include <string>
#include <map>

struct ini : std::map<std::string, fmt::map>
{
	std::string head;

	std::istream& feed(std::istream& in)
	{
		std::string s;
		if (fmt::getline(in, s))
		{
			if (is_head(s))
			{
				head = s.substr(1, s.size() - 1);
			}
			else
			{
				auto [key, value] = fmt::key_value(s);
				at(head).insert(key, value);
			}
		}
	}

	std::string_view operator()(std::string_view u) const
	{
		return at(head)[u];
	}

private:

	static bool is_head(std::string_view s)
	{
		return not empty(s) and s.front() == '[' and s.back() == ']';
	}
};

#endif file