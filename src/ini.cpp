// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ini.hpp"
#include "fmt.hpp"
#include "tag.hpp"
#include "type.hpp"
#include "dig.hpp"
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
	fmt::view::in::ref ini::getline(view::in::ref input, string::ref output)
	{
		while (std::getline(input, output))
		{
			constexpr char omit = '#';
			// Read past comment
			view u = output;
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
					output = fmt::to_string(u);
					break; // done
				}
			}
		}
		return input;
	}

	fmt::view::in::ref operator>>(fmt::view::in::ref input, ini::ref output)
	{
		long group = 0;
		ini::string token;

		while (ini::getline(input, token))
		{
			// Check for new group
			if (header(token))
			{
				const auto z = token.size();
				token = token.substr(1, z - 2);
				group = fmt::tag::set(token);
				continue;
			}

			#ifdef trace
			if (not fmt::tag::got(group))
			{
				trace("no group");
			}
			#endif

			// Create key pair for value entry
			const auto pair = fmt::to_pair(token);
			const auto key = fmt::tag::set(pair.first);
			const auto value = pair.second;
			if (value.empty())
			{
				break;
			}

			// Create a new entry in the key table
			if (not output.set({ group, key }, value))
			{
				#ifdef trace
				trace("overwrite", key, "with", value);
				#endif
			}
		}
		return input;
	}

	fmt::view::out::ref operator<<(fmt::view::out::ref output, ini::cref input)
	{
		long last = -1;
		for (auto [k, v] : input.keys)
		{
			if (k.first != last)
			{
				last = k.first;

				const auto group = fmt::tag::get(k.first);
				output << '[' << group << ']' << fmt::eol;
			}

			const auto key = fmt::tag::get(k.second);
			const fmt::view value = input.values.at(v);
			output << key << "=" << value << fmt::eol;
		}
		return output;
	}

	bool ini::got(pair key) const
	{
		return keys.find(key) != keys.end();
	}

	ini::view ini::get(pair key) const
	{
		const auto it = keys.find(key);
		return it != keys.end()
			? values.at(it->second)
			: fmt::empty;
	}

	bool ini::set(pair key, view value)
	{
		const auto it = cache.emplace(value).first;
		assert(cache.end() != it);
		return put(key, *it);
	}

	bool ini::put(pair key, view value)
	{
		const auto it = keys.find(key);
		if (keys.end() == it)
		{
			const auto size = values.size();
			values.emplace_back(value);
			keys[key] = fmt::to_int(size);
			return true;
		}
		else
		{
			values[it->second] = value;
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
			fmt::string::view u = value;
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