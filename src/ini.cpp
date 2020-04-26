#include "ini.hpp"
#include "str.hpp"
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

	ini::in::ref ini::getline(in::ref input, string::ref output)
	{
		while (std::getline(input, output))
		{
			// Read past comment
			constexpr char omit = '#';
			auto const beign = output.begin();
			auto const end = output.end();
			auto const it = fmt::skip(begin, end);
			if (it != end and omit != *it)
			{
				// Trim off whitespace
				auto const t = output.find(omit);
				output = output.substr(0, t);
				auto const u = fmt::trim(output);
				if (not empty(u))
				{
					output = fmt::to_string(u);
					break; // done
				}
			}
		}
		return in;
	}

	ini::in::ref operator>>(ini::in::ref input, ini::ref output)
	{
		word group = -1;
		ini::string token;

		while (ini::getline(input, token))
		{
			// Check for new group
			if (header(token))
			{
				auto const z = token.size();
				token = token.substr(1, z - 2);
				group = fmt::str::get(token);
				continue;
			}

			#ifdef trace
			if (group < 0)
			{
				trace("no group");
			}
			#endif

			// Create key pair for value entry
			auto const pair = fmt::to_pair(token);
			word const key = fmt::str::get(pair.first);
			view const value = pair.second;
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
		return in;
	}

	ini::out::ref operator<<(ini::out::ref output, ini::cref input)
	{
		for (auto it : input.keys)
		{
			if (it.first < 0)
			{
				continue;
			}

			view const group = fmt::str::get(it.first);
			out << '[' << group << ']' << fmt::eol;
			
			for (auto jt : it.second)
			{
				view const key = fmt::str::get(jt.first);
				view const value = input.cache.at(jt.second);

				out << key << "=" << value << fmt::eol;
			}
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

