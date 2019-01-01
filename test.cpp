#include "test.hpp"
#include "str.hpp"
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <exception>
#include <algorithm>
#include <string>
#include <vector>
#include <regex>
#include <map>

namespace test
{
	namespace
	{
		std::map<unit *, std::string_view> registry;
	}

	unit::unit(char const *name)
	{
		[[maybe_unused]] auto const [it, unique] = registry.emplace(this, name);
		assert(unique);
	}

	unit::~unit()
	{
		[[maybe_unused]] auto const count = registry.erase(this);
		assert(1 == count);
	}

	int run(int argc, char **argv)
	{
		std::vector<std::string> errors;

		std::cout << test::registry.size() << " tests to run.\n";

		for (auto const [ptr, name] : test::registry) try
		{
			std::cout << "Running test '" << name << "' ... ";
			ptr->run();
			std::cout << "Success\n";
		}
		catch (std::exception const& except)
		{
			std::cout << "Failure\n";
			errors.emplace_back(name);
			auto message = except.what();
			errors.emplace_back(message);
		}
		catch (...)
		{
			std::cerr << "Unknown exception\n";
		}

		std::cout << "Done running tests.\n";

		if (not errors.empty())
		{
			std::cout << errors.size() << " errors recorded:\n";
			std::ostream_iterator<std::string> out(std::cout, "\n");
			std::copy(errors.begin(), errors.end(), out);
		}
		else
		{
			std::cout << "No errors.\n";
		}

		return errors.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
	}
}

