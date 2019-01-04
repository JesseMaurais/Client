#include "dbg.hpp"
#include "ios.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <string>
#include <map>

namespace
{
	using map = std::map<debug::test*, std::string>;
	std::string::size_type max_length = 0;

	map& registry()
	{
		static map instance;
		return instance;
	}

	//TEST(dumb_assert, ASSERT(false));
	//TEST(dumb_except, throw std::runtime_error("Holy Cow!"));
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

	int run(int argc, char **argv)
	{
		using namespace io;
		auto& out = std::cout;
		constexpr auto eol = '\n';

		map const& tests = registry();
		out << intense << tests.size() << " tests to run..." << intense_off << eol;

		unsigned int errors = 0;
		for (auto const& [that, name] : tests) try
		{
			auto const indent = max_length - name.length();
			out << faint << name << intense_off << std::string(indent, ' ');
			that->run();
			out << fg_green << "\tok" << fg_off << eol;
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

