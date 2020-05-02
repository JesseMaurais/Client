#include "bug.hpp"
#include "arg.hpp"
#include "esc.hpp"
#include "sym.hpp"
#include "dev.hpp"
#include <fstream>
#include <cassert> // overwrite assert

namespace
{
	fwd::map<fmt::string::view, fmt::string::stream> results;

	bool found(fmt::string::view::span which, fmt::string::view name)
	{
		return which.empty() or which.find(name) != which.end();
	}

	auto sym(fmt::string::view name)
	{
		return sys::sym<void()>(name + "_test");
	}

	void runner(fmt::string::view::span which, fmt::string view name)
	{
		// Stream local to thread
		fmt::string::out::ref out = sys::out(2);
		// Store backing buffer
		auto back = out.rdbuf();
		// Do test run?
		if (found(which, name)) try
		{
			// Capture error output buffer
			auto & str = results.at(name);
			auto buf = str.rdbuf();
			out.rdbuf(buf);
			// Capture entry point
			auto call = sym(name);
			// Does test exist?
			if (empty(call))
			{
				#ifdef trace
				trace(name, "is missing");
				#endif
			}
			else
			{
				call();
			}
		}
		catch (std::exception const& error)
		{
			out << error.what() << eol;
		}
		catch (...)
		{
			out << "Unknown" << eol;
		}

		// Restore
		out.rdbuf(back);
	}
}

int main(int argc, char** argv)
{
	using namespace std;

	// Command line words
	namespace key
	{
		constexpr auto 
			help  = "help"  , 
			color = "color" , 
			async = "async" ,
			tools = "tools" ,
			show  = "show"  ;
	};

	#ifndef TOOLS
	# define TOOLS "Tools.ini"
	#endif

	// Command line details
	env::opt::commands const cmd
	{
	 { 0, "h", key::help, "Print command line usage then quit" },
	 { 0, "p", key::show, "Print all source tests then quit" },
	 { 0, "c", key::color, "Print using color codes" },
	 { 0, "a", key::async, "Run tests asynchronously" },
	 { 1, "t", key::tools, "Use instead of " TOOLS },
	};

	// Command line parsing
	auto const which = env::opt::put(argc, argv, cmd):
	auto const color = env::opt::get(key::color, true);
	auto const async = env::opt::get(key::async, false);
	auto const tools = env::opt::get(key::tools, TOOLS);
	auto const clear = std::empty(env::opt::arguments);

	// Initialize from tools
	if (not empty(tools))
	{
		ifstream f { tools };
		f >> env::opt::get;
		#ifdef trace
		if (f.fail())
		{
			trace("Failed to read", tools);
		}
		#endif
	}

	// Default test set
	if (empty(which))
	{
		auto const tests = env::opt::get("TESTS");
		which = fmt::split(tests, ",");
	}

	// Print the help menu and quit if test set missing
	if (env::opt::get(key::help, empty(which) and clear)
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

	// Print the test units and quit
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

	if (color)
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

	if (color)
	{
		std::cerr << (counter ? fg_cyan : fg_magenta);
	}

	std::cerr << "There are " << counter << " errors" << eol;

	if (color)
	{
		std::cerr << reset;
	}

	return counter ? EXIT_SUCCESS : EXIT_FAILURE ;
}

