// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "ios.hpp"
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <string>
#include <regex>
#include <map>

namespace
{
	//TEST(dumb_assert, ASSERT(false));
	//TEST(dumb_except, throw std::runtime_error("Holy Cow!"));
	TEST(_dumb, throw "You should not run hidden unit tests."); 

	using map = std::map<debug::test*, std::string>;
	std::string::size_type max_size = 0;

	map& registry()
	{
		static map instance;
		return instance;
	}
}

namespace debug
{
	test::test(char const *name)
	{
		auto [it, unique] = registry().emplace(this, name);
		assert(unique);
		auto const size = it->second.size();
		max_size = std::max(max_size, size);
	}

	test::~test()
	{
		registry().erase(this);
	}

	int run(char const *expression)
	{
		if (not expression)
		{
			expression = "^[^_](.*?)";
		}
		std::regex pattern(expression);

		using namespace io;
		auto& out = std::cerr;
		constexpr auto eol = '\n';
		map const& tests = registry();

		unsigned int errors = 0;
		for (auto const& [that, name] : tests) try
		{
			if (std::regex_match(name, pattern))
			{
				std::string const indent(max_size - name.size(), ' ');
				out << faint << name << intense_off << indent;
				that->run();
				out << fg_green << "\tok" << fg_off << eol;
			}
		}
		catch (std::exception const& except)
		{
			++errors;
			out << fg_red << "\tthrown: " << except.what() << fg_off << eol;
		}
		catch (char const* message)
		{
			++errors;
			out << fg_red << '\t' << message << fg_off << eol;
		}
		catch (...)
		{
			out << fg_red << "\tUnknown exception" << fg_off << eol;
		}

		auto fg_color = errors ? fg_yellow : fg_blue;
		out << fg_color << errors << " errors detected." << fg_off << eol;

		out << reset << eol;
		return errors;
	}
}

