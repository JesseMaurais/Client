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
	ini::string ini::join(span list)
	{
		return fmt::join(list, separator);
	}

	ini::vector ini::split(view line)
	{
		return fmt::split(line, separator);
	}

	ini::string ini::join(init list)
	{
		return fmt::join(list, separator);
	}

	ini::in::ref ini::getline(in::ref input, string::ref output)
	{
		while (std::getline(input, output))
		{
			// Read past comment
			constexpr char omit = '#';
			auto const begin = output.begin();
			auto const end = output.end();
			auto const it = fmt::skip(begin, end);
			if (it != end and omit != *it)
			{
				// Trim off whitespace
				auto const t = output.find(omit);
				output = output.substr(0, t);
				view const u = fmt::trim(output);
				if (not u.empty())
				{
					output = fmt::to_string(u);
					break; // done
				}
			}
		}
		return input;
	}

	ini::in::ref operator>>(ini::in::ref input, ini::ref output)
	{
		path::type group = 0;
		ini::string token;

		while (ini::getline(input, token))
		{
			// Check for new group
			if (header(token))
			{
				auto const z = token.size();
				token = token.substr(1, z - 2);
				group = fmt::tag::set(token);
				continue;
			}

			#ifdef trace
			if (not fmt::got(group))
			{
				trace("no group");
			}
			#endif

			// Create key pair for value entry
			auto const pair = fmt::to_pair(token);
			auto const key = fmt::tag::set(pair.first);
			auto const value = pair.second;
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

	ini::out::ref operator<<(ini::out::ref output, ini::cref input)
	{
		path::type last = -1;
		for (auto [k, v] : input.keys)
		{
			if (k.first != last)
			{
				last = k.first;

				auto const group = fmt::get(k.first);
				output << '[' << group << ']' << fmt::eol;
			}

			auto const key = fmt::get(k.second);
			ini::view const value = input.values.at(v);
			output << key << "=" << value << fmt::eol;
		}
		return output;
	}

	bool ini::got(path::pair key) const
	{
		return keys.find(key) != keys.end();
	}

	ini::view ini::get(path::pair key) const
	{
		auto const it = keys.find(key);
		return it == keys.end() ? "" : values.at(it->second);
	}

	bool ini::set(path::pair key, view value)
	{
		auto const it = cache.emplace(value).first;
		assert(cache.end() != it);
		return put(key, *it);
	}

	bool ini::put(path::pair key, view value)
	{
		auto const it = keys.find(key);
		if (keys.end() == it)
		{
			auto const size = values.size();
			values.emplace_back(value);
			keys[key] = fmt::to<path::type>(size);
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