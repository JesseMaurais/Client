#include "test.hpp"
#include "sh.hpp"
#include <exception>
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

	//TEST(dumb, ASSERT(false));
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

		std::cout << sh::intense << tests.size() << " tests to run..." << sh::intense_off << '\n';

		int errors = 0;

		for (auto const& [that, name] : tests) try
		{
			auto const indent = max_length - name.length();
			std::cout << name << std::string(indent, ' ');
			that->run();
			std::cout << sh::fg_green << "\tok" << sh::fg_off << '\n';
		}
		catch (std::exception const& except)
		{
			++errors;
			std::cout << sh::fg_red << "\tthrown: " << except.what() << sh::fg_off << '\n';
		}
		catch (char const* message)
		{
			++errors;
			std::cout << sh::fg_red << '\t' << message << sh::fg_off << '\n';
		}
		catch (...)
		{
			std::cerr << "\tUnknown exception\n";
		}

		auto fg = errors ? sh::fg_yellow : sh::fg_cyan;
		auto tense = errors ? sh::intense : sh::faint;
		std::cout << fg << tense << errors << " errors detected." << sh::reset << std::endl;
		return errors;
	}
}

