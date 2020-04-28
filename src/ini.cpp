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
		word group = 0;
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
			if (not fmt::str::got(group))
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
		word last = -1;
		for (auto [k, v] : input.keys)
		{
			if (key.first != last)
			{
				last = key.first;

				view const group = fmt::str::get(k.first);
				out << '[' << group << ']' << fmt::eol;
			}
			
			view const key = fmt::str::get(k.second);
			view const value = input.values.at(v);
			out << key << "=" << value << fmt::eol;
		}
		return out;
	}

	bool ini::got(pair key) const
	{
		return keys.find(key) != keys.end();
	}

	view ini::get(pair key) const
	{
		auto const it = keys.find(key);
		return it == keys.end() ? fmt::nil : it->second;
	}

	bool ini::set(pair key, view value)
	{
		auto it = keys.find(key);
		if (keys.end() == it)
		{
			auto const size = values.size();
			keys.at(key) = to<word>(size);
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

