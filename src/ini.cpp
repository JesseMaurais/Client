#include "ini.hpp"
#include "key.hpp"
#include "str.hpp"
#include "err.hpp"

namespace
{
	bool header(fmt::string::view u)
	{
		return not empty(u) and u.front() == '[' and u.back() == ']';
	}

	constexpr auto separator = ";";
}

namespace doc
{
	auto ini::join(span list)
	{
		return fmt::join(list, separator);
	}

	auto ini::split(view line)
	{
		return fmt::split(line, separator);
	}

	auto ini::join(init list)
	{
		return fmt::join(list, separator);
	}

	ini::in::ref ini::getline(in::ref in, string::ref line)
	{
		while (std::getline(in, line))
		{
			constexpr char omit = '#';
			auto const it = fmt::skip(begin(line), end(line));
			if (it != end(s) and omit != *it)
			{
				auto const t = line.find(omit);
				line = line.substr(0, t);
				auto const u = fmt::trim(line);
				if (not empty(u))
				{
					line = fmt::to_string(u);
					break;
				}
			}
		}
		return in;
	}

	ini::in::ref operator>>(ini::in::ref in, ini::ref group)
	{
		ini::string key, line;
		while (ini::getline(in, line))
		{
			if (header(line))
			{
				auto const z = line.size();
				key = line.substr(1, z - 2);
				if (empty(key)) break;
				continue;
			}

			if (empty(key))
			{
				sys::warn(here, "no key");
				break;
			}

			auto const p = fmt::to_pair(line);
			pair const e { key, p.first };
			if (not keys.put(e, p.second))
			{
				sys::warn(here, "overwrite", key, "with", p.second);
			}
		}
		return in;
	}

	ini::out::ref operator<<(ini::out::ref out, ini::cref obj)
	{
		view key;
		for (auto const& it : obj.keys)
		{
			if (it.first.first != key)
			{
				key = it.first.first;
				out << '[' << key << ']' << fmt::eol;
			}
			out << it.first.second << "=" << it.second;
		}
		return out;
	}

	bool ini::got(pair key) const
	{
		auto const it = keys.find(key.first);
		if (keys.end() != it)
		{
			for (auto const item : it->second)
			{
				if (item.first == key.second)
				{
					return true;
				}
			}
		}
		return false;
	}

	view ini::get(pair key) const
	{
		auto const it = keys.find(key.first);
		auto const end = keys.end();
		if (end != it)
		{
			for (auto const item : it->second)
			{
				if (item.first == key.second)
				{
					return item.second;
				}
			}
		}
		return fmt::nil;
	}

	bool ini::put(pair key, view value)
	{
		key.first = env::opt::get(key.first);
		key.second = env::opt::get(key.second);
		value = env::opt::get(value);
		return set(key, value);
	}

	bool ini::set(pair key, view value)
	{
		auto [it, unique] = keys.insert_or_assign(key.first);
		it->second.emplace_back(fmt::to_pair(key.second, value));
		return unique;
	}

	bool ini::cut(pair key)
	{
		auto it = keys.find(key);
		if (keys.end() != it)
		{
			keys.erase(it);
			return true;
		}
		return false;
	}
}

