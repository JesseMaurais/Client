#include "test.hpp"
#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <string_view>
#include <regex>
#include <map>

namespace test
{
	namespace
	{
		std::map<interface *, std::string_view> registry;
	}

	interface::interface(char const *name)
	{
		[[maybe_unused]] auto const [it, removed] = registry.emplace(this, name);
		assert(not removed);
	}

	interface::~interface()
	{
		[[maybe_unused]] auto const count = registry.erase(this);
		assert(1 == count);
	}
}

int main(int argc, char **argv)
{
	std::queue<std::string> errors;

	for (auto const [ptr, name] : registry) try
	{
		ptr->run();
	}
	catch (std::exception const &except)
	{
		auto const message = except.what();
		errors.emplace(message);
	}
	catch (...)
	{
		errors.emplace("?");
	}

	{
		auto const begin = errors.begin();
		auto const end = errors.end();
		std::sort(begin, end);
		auto const cut = std::unique(begin, end);
		errors.erase(cut, end);
		std::ostream_iterator<std::string> out(std::out, "\n");
		std::copy(begin, cut, out); 
	}

	return errors.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

