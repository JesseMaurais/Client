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

	using map = std::map<debug::test*, std::string>;
	std::string::size_type max_length = 0;

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
		registry().emplace(this, name);
		auto const length = std::string(name).length();
		max_length = std::max(max_length, length);
	}

	test::~test()
	{
		registry().erase(this);
	}

	int run(int argc, char** argv)
	{
		char const* expression = 1 < argc and argv[1] ? argv[1] : "(.*?)";
		std::regex pattern(expression);

		using namespace io;
		auto& out = std::cout;
		constexpr auto eol = '\n';

		map const& tests = registry();
		out << intense << tests.size() << " tests to run..." << intense_off << eol;

		unsigned int errors = 0;
		for (auto const& [that, name] : tests) try
		{
			if (std::regex_match(name, pattern))
			{
				std::string const indent(max_length - name.length(), ' ');
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

		auto fg = errors ? fg_yellow : fg_blue;
		out << fg << errors << " errors detected." << fg_off << eol;

		out << reset << eol;
		return errors;
	}
}

