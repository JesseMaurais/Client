#include "ini.hpp"
#include "fmt.hpp"
#include "err.hpp"

namespace ini
{
	istream & getline(istream & in, string & s, char c)
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

	bool header(view u)
	{
		return not empty(u) and u.front() == '[' and u.back() == ']';
	}

	keys::keys(istream & in)
	{
		string key;
		string line;
		while (getline(in, line))
		{
			if (header(line))
			{
				key = line.substr(1, size(line) - 2);
				continue;
			}

			assert(not empty(key));
			auto const p = fmt::to_pair(line);
			pair const e { key, p.first };
			if (set(e, p.second))
			{
				sys::warn(here, "overwite", key, "with", p.second);
			}
		}
	}

	view keys::get(pair key) const
	{
		auto const it = map.find(key);
		if (map.end() != it)
		{
			return it->second;
		}
		return "";
	}

	bool keys::set(pair key, view value)
	{
		return not map.insert_or_assign(key, value).second;
	}
}

