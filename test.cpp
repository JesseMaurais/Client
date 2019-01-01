#include "test.hpp"
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

		std::cout << tests.size() << " tests to run...\n";

		int errors = 0;

		for (auto const& [that, name] : tests) try
		{
			auto const indent = max_length - name.length();
			std::cout << name << std::string(indent, ' ');
			that->run();
			std::cout << "\tok\n";
		}
		catch (std::exception const& except)
		{
			std::cout << "\tthrown: " << except.what() << '\n';
			++errors;
		}
		catch (char const* message)
		{
			std::cout << '\t' << message << '\n';
			++errors;
		}
		catch (...)
		{
			std::cerr << "\tUnknown exception\n";
		}

		std::cout << errors << " errors detected." << std::endl;
		return errors;
	}
}

