// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ini.hpp"
#include "str.hpp"
#include "dir.hpp"
#include "dig.hpp"
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
				if (not empty(u))
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
		env::opt::word group = 0;
		ini::string token;

		while (ini::getline(input, token))
		{
			// Check for new group
			if (header(token))
			{
				auto const z = token.size();
				token = token.substr(1, z - 2);
				group = fmt::str::set(token);
				continue;
			}

			#ifdef trace
			if (not fmt::str::got(group))
			{
				trace("no group");
			}
			#endif

			// Create key pair for value entry
			auto const pair = fmt::to_pair(token);
			auto const key = fmt::str::set(pair.first);
			auto const value = pair.second;
			if (empty(value))
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
		env::opt::word last = -1;
		for (auto [k, v] : input.keys)
		{
			if (k.first != last)
			{
				last = k.first;

				auto const group = fmt::str::get(k.first);
				output << '[' << group << ']' << fmt::eol;
			}
			
			auto const key = fmt::str::get(k.second);
			ini::view const value = input.values.at(v);
			output << key << "=" << value << fmt::eol;
		}
		return output;
	}

	bool ini::got(env::opt::pair key) const
	{
		return keys.find(key) != keys.end();
	}

	ini::view ini::get(env::opt::pair key) const
	{
		auto const it = keys.find(key);
		return it == keys.end() ? "" : values.at(it->second);
	}

	bool ini::set(env::opt::pair key, view value)
	{
		auto it = keys.find(key);
		if (keys.end() == it)
		{
			auto const size = values.size();
			keys.at(key) = fmt::to<env::opt::word>(size);
			values.emplace_back(value);
			return true;
		}
		else
		{
			values.at(it->second) = value;
			return false;
		}
	}
}

#ifdef test
test(ini)
{
	auto const path = fmt::dir::join({env::pwd(), "Tools.ini"});
	std::fstream file(path);
	doc::ini init;
	file >> init;

	auto const group = fmt::str::set("NMAKE");
	auto const key = fmt::str::set("MAKECONFIG");
	auto const value = init.get({group, key});
	assert(not empty(value));
	assert(value.find("/D_NMAKE") != fmt::npos);
}
#endif
