#include "test.hpp"
#include <exception>
#include <iostream>
#include <cstring>
#include <string>
#include <set>
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
}

namespace debug
{
	test::test(char const *name)
	{
		registry().emplace(this, name);
		auto const length = std::strlen(name);
		max_length = std::max(max_length, length);
	}

	test::~test()
	{
		registry().erase(this);
	}

	int run(int argc, char **argv)
	{
		std::set<std::string> errors;

		map const& tests = registry();

		std::cout << tests.size() << " tests to run...\n";

		for (auto const& [that, name] : tests) try
		{
			auto const indent = max_length - name.length();
			std::cout << name << std::string(indent, ' ');
			that->run();
			std::cout << "\tok\n";
		}
		catch (std::exception const& except)
		{
			std::cout << "\tTHROWN\n";
			errors.emplace(except.what());
		}
		catch (char const* message)
		{
			std::cout << "\tFAILURE\n";
			errors.emplace(message);
		}
		catch (...)
		{
			std::cerr << "\tUnknown exception\n";
		}

		auto const n = errors.size();
		std::cout << n << " errors recorded" << (n ? ":" : ".") << "\n";
		for (auto const& message : errors)
		{
			std::cout << message << '\n';
		}
		std::cout << std::endl;
		return n;
	}
}

