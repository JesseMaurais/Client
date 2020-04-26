#include "ini.hpp"
#include "key.hpp"
#include "type.hpp"
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

	ini::in::ref ini::getline(in::ref input, string::ref line)
	{
		while (std::getline(input, line))
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

	ini::in::ref operator>>(ini::in::ref in, ini::ref obj)
	{
		ini::view key;
		ini::string line;
		while (ini::getline(in, line))
		{
			if (header(line))
			{
				auto const z = line.size();
				auto const u = line.substr(1, z - 2);
				key = env::opt::set(u);
			}

			if (empty(key))
			{
				sys::warn(here, "no key");
			}

			auto pair = fmt::to_pair(line);
			pair.first = fmt::str::get(pair.first);
			pair.second = fmt::str::get(pair.second);
			
			if (not obj.set({ key, pair.first }, pair.second))
			{
				sys::warn(here, "overwrite", key, "with", pair.second);
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
		if (key.end() != it)
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
		if (keys.end() != it)
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

	bool ini::set(pair key, view value)
	{
		auto const it = keys.find(key.first);
		if (keys.end() != it)
		{
			for (auto const item : it->second)
			{
				if (item.first == key.second)
				{
					item.second = cache.emplace(value);
					return true;
				}
			}
		}
		return false
	}

	bool ini::put(pair key, view value)
	{
		auto it = keys.find(key.first);
		if (keys.end() == it)
		{
			for (auto const item : it->second)
			{
				if (item.first == key.second)
				{
					item.second = value;
					return true;
				}
			}
		}
		return false;
	}
}

