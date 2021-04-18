// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "meta.hpp"
#include "msg.hpp"
#include "opt.hpp"
#include "str.hpp"
#include "ini.hpp"
#include "err.hpp"
#include "type.hpp"

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
		env::opt::name group = 0;
		ini::string token;

		while (ini::getline(input, token))
		{
			// Check for new group
			if (header(token))
			{
				auto const z = token.size();
				token = token.substr(1, z - 2);
				group = fmt::set(token);
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
			auto const key = fmt::set(pair.first);
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
		env::opt::name last = -1;
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
		auto const it = cache.emplace(value).first;
		assert(cache.end() != it);
		return put(key, *it);
	}

	bool ini::put(env::opt::pair key, view value)
	{
		auto const it = keys.find(key);
		if (keys.end() == it)
		{
			auto const size = values.size();
			values.emplace_back(value);
			keys[key] = fmt::to<env::opt::name>(size);
			return true;
		}
		else
		{
			values[it->second] = value;
			return false;
		}
	}

	template class instance<function>;
}

#ifdef test_unit

#include "env.hpp"
#include "dir.hpp"

namespace
{
	using namespace std::literals::string_view_literals;

	struct dumb
	{
		int i = 0;
		long n = 0;
		short w = 0;
		char b = 0;
		float f = 0.0f;
		double d = 0.0;
		fmt::string s = "Hello World";

		static constexpr auto table()
		{
			return std::tuple
			{
				&dumb::i,
				&dumb::f,
				&dumb::s,
			};
		}
	};
}

template class doc::instance<dumb>;
template <> fmt::string::view doc::name<&dumb::i> = "i";
template <> fmt::string::view doc::name<&dumb::f> = "f";
template <> fmt::string::view doc::name<&dumb::s> = "s";

test_unit(doc)
{
	using parent_type = fwd::offset_of<&dumb::i>::parent_type;
	static_assert(std::is_same<parent_type, dumb>::value);

	using value_type = fwd::offset_of<&dumb::i>::value_type;
	static_assert(std::is_same<value_type, int>::value);

	size_t const id = doc::access<dumb>().emplace({});
	assert(0 == id);
	auto ptr = doc::access<dumb>().find(id);
	assert(nullptr != ptr);

	doc::value<0>(ptr) = 42;
	doc::value<1>(ptr) = 4.2f;
	doc::value<2>(ptr) = "42";

	assert(42 == ptr->i);
	assert(4.2f == ptr->f);
	assert(ptr->s == "42");

	assert(42 == doc::value<0>(ptr));
	assert(4.2f == doc::value<1>(ptr));
	assert(doc::value<2>(ptr) == "42");

	assert(doc::key<0>(ptr) == "i"sv);
	assert(doc::key<1>(ptr) == "f"sv);
	assert(doc::key<2>(ptr) == "s"sv);

	doc::access<dumb>().free(id);
}

test_unit(ini)
{
	auto const path = fmt::dir::join({env::pwd(), ".ini"});
	std::fstream file(path);
	doc::ini init;
	file >> init;

	// Data from file
	{
		auto const group = fmt::set("NMAKE");
		auto const key = fmt::set("MAKECONFIG");
		auto const value = init.get({group, key});
		assert(not empty(value));
		assert(value.find("/D_NMAKE") != fmt::npos);
	}

	// Data at runtime
	{
		auto const group = fmt::set("Group");
		auto const key = fmt::set("Key");
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
