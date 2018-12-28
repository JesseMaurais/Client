#ifndef ini_hpp
#define ini_hpp

#include <utility>
#include <vector>
#include <istream>
#include <string_view>
#include <string>
#include <map>

struct ini
{
	using istream = std::istream;
	using string = std::string;
	using view = std::string_view;
	using entry = std::pair<string, string>;
	using keys = std::vector<entry>;
	using group = std::pair<string, keys>;
	using map = std::map<string, group>;

	constexpr auto npos = string::npos;

	static bool is_header(string_view s)
	{
		auto const begin = s.find_first_of('[');
		auto const end = s.find_last_of(']');

		if (begin < end and npos != end)
		{
			if (s.find_first_of(']'), begin) == end)
			{
				if (s.find_last_of('[', end) == begin)
				{
					return true;
				}
			}
		}
		return false;
	}

	istream& feed(istream& in)
	{
		string s;
		if (fmt::getline(in, s))
		{
			if (not is_header(s))
			{
				string [key, value] = fmt::key_value(s);
				fmt::trim(key);
				fmt::trim(value);
				data[head].insert(key, value);
			}
			else head = s;
		}
	}

private:

	map data;
	string head;
};

#endif file