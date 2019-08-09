#include "ini.hpp"
#include "fmt.hpp"
#include "err.hpp"
#include <fstream>
#include <tuple>

namespace ini
{
	istream& getline(istream& in, string& s, char c)
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

	bool header(string_view u)
	{
		return not empty(u) and u.front() == '[' and u.back() == ']';
	}

	keys::keys(string_view path)
	{
		auto s = fmt::to_string(path);
		std::ifstream in(s);
		string head;
		while (getline(in, s))
		{
			if (header(s))
			{
				head = s;
				continue;
			}

			assert(not empty(head));
			auto const p = fmt::to_pair(s);
			ini::entry e { head, p.first };
			put(e, p.second);
		}
	}

	value keys::get(entry e) const
	{
		auto const node = tree.find(e.first);
		if (tree.end() != node)
		{
			auto const it = node->second.find(e.second);
			if (node->second.end() != it)
			{
				return it->second;
			}
		}
		return "";
	}

	void keys::put(entry e, value u)
	{
		auto const h = store(e.first);
		auto const k = store(e.second);
		auto const v = store(u);
		(void) tree[h].insert_or_assign(k, v);
	}

	value keys::store(value u)
	{
		auto s = fmt::to_string(u);
		auto it = buf.find(s);
		if (end(buf) == it)
		{
			std::tie(it, std::ignore) = buf.emplace(std::move(s));
			assert(buf.end() != it);
		}
		return end(buf) == it ? "" : *it;
	}
}

