#include "ini.hpp"
#include "fmt.hpp"
#include "err.hpp"
#include <tuple>

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
		view key;
		string line;
		while (getline(in, line))
		{
			buf.emplace_back(line);
			const auto& it = buf.back();;

			if (header(it))
			{
				key = view(data(it), size(it));
				bool eof = key == "eof";
				if (eof) break;
				else continue;
			}

			assert(not empty(key));

			auto const p = fmt::to_pair(it);
			auto const q = std::make_pair(key, p.first);
			put(q, p.second);
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

