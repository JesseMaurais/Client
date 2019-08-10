#include "ini.hpp"
#include "fmt.hpp"
#include "err.hpp"
#include <utility>

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
				key = move(line);
				continue;
			}

			assert(not empty(key));
			auto const p = fmt::to_pair(line);
			entry const e { key, p.first };
			put(e, p.second);
		}
	}

	view keys::get(entry e) const
	{
		auto const node = tree.find(e.first);
		if (tree.end() != node)
		{
			auto const& that = node->second;
			auto it = that.find(e.second);
			if (that.end() != it)
			{
				return it->second;
			}
		}
		return "";
	}

	void keys::put(entry e, view u)
	{
		(void) tree[e.first].insert_or_assign(e.second, u);
	}
}

