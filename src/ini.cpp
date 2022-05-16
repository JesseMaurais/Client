// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ini.hpp"
#include "fmt.hpp"
#include "tag.hpp"
#include "err.hpp"
#include "type.hpp"

namespace
{
	bool header(fmt::view u)
	{
		return not u.empty() and u.front() == '[' and u.back() == ']';
	}
}

namespace doc
{
	fmt::input ini::getline(fmt::input buf, fmt::string::ref line)
	{
		while (std::getline(buf, line))
		{
			constexpr char omit = '#';
			// Read past comment
			view u = line;
			const auto begin = u.begin();
			const auto end = u.end();
			const auto it = fmt::skip(begin, end);
			if (it != end and omit != *it)
			{
				// Trim off whitespace
				const auto t = u.find(omit);
				u = u.substr(0, t);
				u = fmt::trim(u);
				if (not u.empty())
				{
					line = fmt::to_string(u);
					break; // done
				}
			}
		}
		return buf;
	}

	fmt::input operator>>(fmt::input buf, ini::ref data)
	{
		fmt::string token;
		auto group = fmt::tag::empty;
		while (ini::getline(buf, token))
		{
			// Check for new group
			if (header(token))
			{
				token = token.substr(1, token.size() - 2);
				group = fmt::tag::set(token);
				continue;
			}
			// Create key pair for value entry
			const auto pair = fmt::to_pair(token);
			const auto key = fmt::tag::set(pair.first);
			const auto value = pair.second;
			if (value.empty())
			// Create a new entry in the key table
			data.set({ group, key }, value);
		}
		return buf;
	}

	fmt::output operator<<(fmt::output buf, ini::cref data)
	{
		auto last = fmt::tag::empty;
		for (auto [key, value] : data.keys)
		{
			if (key.first != last)
			{
				last = key.first;
				buf << '[' << key.first << ']' << fmt::tag::eol;
			}
			buf << key.second << fmt::tag::assign << value << fmt::tag::eol;
		}
		return buf;
	}

	bool ini::got(pair key) const
	{
		return keys.find(key) != keys.end();
	}

	ini::view ini::get(pair key) const
	{
		const auto it = keys.find(key);
		return keys.end() == it
			? fmt::tag::empty : it->second;
	}

	bool ini::set(pair key, view value)
	{
		const auto it = cache.emplace(value).first;
		#ifdef assert
		assert(cache.end() != it);
		#endif
		return put(key, *it);
	}

	bool ini::put(pair key, view value)
	{
		const auto it = keys.find(key);
		if (keys.end() == it)
		{
			keys[key] = values.emplace_back(value);
			return true;
		}
		else
		{
			it->second = value;
			return false;
		}
	}
}

#ifdef test_unit
test_unit(ini)
{
	doc::ini init;

	// Data from file
	/*{
		auto const path = fmt::dir::join({env::pwd(), ".ini"});
		std::fstream file(path);
		file >> init;

		auto const group = fmt::tag::set("NMAKE");
		auto const key = fmt::tag::set("MAKECONFIG");
		auto const value = init.get({group, key});
		assert(not empty(value));
		assert(value.find("/D_NMAKE") != fmt::npos);
	}*/

	// Data at runtime
	{
		auto const group = fmt::tag::set("Group");
		auto const key = fmt::tag::set("Key");
		constexpr auto value = "Value";
		// Cache value with set
		{
			auto s = fmt::to_string(value);
			bool unique = init.set({group, key}, s);
			assert(unique);
			s.clear();
		}
		// Check persistence
		{
			auto u = init.get({group, key});
			assert(not empty(u));
			assert(u == value);
		}
		// Reference with put
		{
			fmt::view u = value;
			bool unique = init.put({group, key}, u);
			assert(not unique);
		}
		// Check persistence
		{
			auto u = init.get({group, key});
			assert(not empty(u));
			assert(u == value);
			assert(u.data() == value);
		}
	}
}
#endif