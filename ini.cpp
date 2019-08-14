#include "ini.hpp"
#include "fmt.hpp"
#include "err.hpp"

using view = ini::view;
using pair = ini::pair;

namespace
{
	bool header(view u)
	{
		return not empty(u) and u.front() == '[' and u.back() == ']';
	}
}

std::istream & ini::getline(std::istream & in, ini::string & s)
{
	constexpr char c = '#';
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

std::istream & operator>>(std::istream & in, ini & keys)
{
	fmt::string key;
	fmt::string line;
	while (ini::getline(in, line))
	{
		if (header(line))
		{
			key = line.substr(1, size(line) - 2);
			if ("---" == key) break;
			continue;
		}

		if (empty(key))
		{
			sys::warn(here, "no key");
			break;
		}

		auto const p = fmt::entry(line);
		pair const e { key, p.first };
		if (not keys.put(e, p.second))
		{
			sys::warn(here, "overwrite", key, "with", p.second);
		}
	}
	return in;
}

std::ostream & operator<<(std::ostream & out, ini & keys)
{
	view key;
	for (auto const i : keys.map)
	{
		if (i.first.first != key)
		{
			key = i.first.first;
			out << '[' << key << ']' << fmt::eol;
		}
		out << i.first.second << "=" << i.second;
	}
	return out;
}

view ini::get(pair key) const
{
	auto const it = map.find(key);
	if (map.end() != it)
	{
		return it->second;
	}
	return fmt::nil;
}

bool ini::set(pair key, view value)
{
	return map.insert_or_assign(key, value).second;
}

bool ini::cut(pair key)
{
	auto it = map.find(key);
	if (map.end() != it)
	{
		auto& value = it->second;
		value.remove_suffix(size(value));
		return true;
	}
	return false;
}

bool ini::put(pair key, view value)
{
	key.first = store(key.first);
	key.second = store(key.second);
	value = store(value);
	return set(key, value);
}

view ini::store(view u)
{
	return *buf.emplace(u).first;
}

