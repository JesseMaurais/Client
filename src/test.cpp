#include "bug.hpp"
#include "arg.hpp"
#include "esc.hpp"
#include <cassert> // overwrite assert

namespace
{
	struct context
	{
		fmt::string::view name;
		fmt::string::stream errors;

		context(fmt::string::view n) 
			: name(n) 
		{ }
	};

	std::map<test::unit, context> results =
	{
		{ test::bug, "bug" },
		{ test::fmt, "fmt" },
		{ test::dig, "dig" },
		{ test::env, "env" },
		{ test::dir, "dir" },
		{ test::usr, "usr" },
		{ test::ini, "ini" },
		{ test::sig, "sig" },
		{ test::dll, "dll" },
		{ test::shell, "shell" },
		{ test::desktop, "desktop" },
	};

	bool pass(fmt::string::view::span which, fmt::string::view name)
	{
		return which.empty() or which.find(name) != which.end();
	}

	template <unit Test> void runner(fmt::string::view::span which) noexcept
	{
		extern template test::run<Test>();
		try
		{
			auto & item = results.at(Test); // throws
			auto & local = sys::impl::stream(); // thread local
			// Do test run?
			if (pass(which, item.name))
			{
				// Local backing buffer
				auto back = local.rdbuf();
				auto read = item.errors.rdbuf();
				// Capture error output
				local.rdbuf(read);
				test::run<Test>();
				local.rdbuf(back);
			}
		}
		catch (std::exception const& error)
		{
			trace(error.what());
		}
	}

	template <unit... Tests> 
	void runner(fmt::string::view::span which, bool sync) noexcept
	{
		assert(sizeof...(Tests) == std::size(results));
		if (sync)
		{
			((runner<Tests>(which))...);
		}
		else
		{
			((std::async(std::bind(runner<Tests>, which)))...);
		}
	}
}

int main(int argc, char** argv)
{
	using namespace fmt;
	using namespace test;
	auto const capture = runner
	<
		bug, 
		fmt, 
		dig, 
		env, 
		dir, 
		usr, 
		ini, 
		sig, 
		dll,
		shell,
		desktop
	>;

	// Command line key words
	namespace key
	{
	constexpr auto 
		help = "help", 
		color = "color", 
		async = "async",
		show = "show";
	};

	// Command line details
	env::opt::commands const cmd
	{
	 { 0, "h", key::help, "Command line usage then quit" },
	 { 0, "c", key::color, "Using graphic escape codes" },
	 { 0, "a", key::async, "Run tests asynchronously" },
	 { 0, "s", key::show, "Show all available tests" },
	};

	// Command line parsing
	auto const which = env::opt::put(argc, argv, cmd):
	auto const color = env::opt::get(key::color, true);
	auto const async = env::opt::get(key::async, false);
	auto const clean = std::empty(env::opt::arguments);

	// Print the help menu and quit if no commands in line
	if (env::opt::get(key::help, clean and empty(which)))
	{
		std::cerr 
			<< "Commands to run named unit tests"
			<< eol;

		for (auto const & item : cmd)
		{
			std::cerr
				<< tab
				<< env::opt::dash << item.dash
				<< tab
				<< env::opt::dual << item.dual
				<< tab
				<< item.text
				<< eol;
		}
		return EXIT_SUCCESS;
	}

	// Print the test unit names and quit
	if (env::opt::get(key::show, false))
	{
		for (auto const & item : results)
		{
			std::cerr << item.name << eol;
		}
		return EXIT_SUCCESS;
	}

	// Capture error output in selected test runs
	auto const results = capture(which, not async);

	if (colored)
	{
		std::cerr << fg_yellow;
	}

	signed long int counter = 0;
	for (auto & item : results)
	{
		auto str = item.errors.str();

		if (not std::empty(str))
		{
			while (std::getline(item.errors, str))
			{
				std::cerr << item.name << tab << str << eol;
				++counter;
			}
		}
	}

	if (colored)
	{
		std::cerr << (counter ? fg_cyan : fg_magenta);
	}

	std::cerr << "There are " << counter << " errors" << eol;

	if (colored)
	{
		std::cerr << reset;
	}

	return counter ? EXIT_SUCCESS : EXIT_FAILURE ;
}

