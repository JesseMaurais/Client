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
		map const& tests = registry();

		std::cout << io::intense << tests.size() << " tests to run..." << io::intense_off << '\n';

		int errors = 0;

		for (auto const& [that, name] : tests) try
		{
			auto const indent = max_length - name.length();
			std::cout << io::faint << name << io::intense_off << std::string(indent, ' ');
			that->run();
			std::cout << io::fg_green << "\tok" << io::fg_off << '\n';
		}
		catch (std::exception const& except)
		{
			++errors;
			std::cout << io::fg_red << "\tthrown: " << except.what() << io::fg_off << '\n';
		}
		catch (char const* message)
		{
			++errors;
			std::cout << io::fg_red << '\t' << message << io::fg_off << '\n';
		}
		catch (...)
		{
			std::cerr << io::fg_red << "\tUnknown exception" << io::fg_off << '\n';
		}

		auto fg = errors ? io::fg_yellow : io::fg_blue;
		std::cout << fg << errors << " errors detected.\n" << io::reset << std::endl;
		return errors;
	}
}

