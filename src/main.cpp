#include "bug.hpp"
#include "arg.hpp"
#include "esc.hpp"
#include "sym.hpp"
#include "dev.hpp"
#include <fstream>
#include <future>
#include <vector>
#include <set>
#include <map>

namespace
{
	fmt::string::view const prefix = "test_";

	void runner(fmt::string::view name, fmt::string::buf::ref buf)
	{
		auto & out = sys::out();
		auto back = out.rdbuf();
		try
		{
			out.rdbuf(buf);
			auto scan = sys::sig::scan
			(
				{ SIGINT, SIGILL, SIGHUP, SIGFPE }
			);
			auto call = sym(name);
			if (nullptr == call)
			{
				out << name << " is missing";
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
			help   = "help"  , 
			color  = "color" , 
			serial = "async" ,
			tools  = "tools" ,
			print  = "print" ;
	};

	#ifndef _TOOLS
	# define _TOOLS "Tools.ini"
	#endif

	// Command line details
	env::opt::commands const cmd
	{
		{ 0, "h", key::help, "Print command line usage then quit" },
		{ 0, "p", key::print, "Print all source tests then quit" },
		{ 0, "c", key::color, "Print using color codes" },
		{ 0, "a", key::serial, "Run tests asynchronously" },
		{ 1, "t", key::tools, "Use instead of " _TOOLS },
	};

	// Command line parsing
	auto const tests = env::opt::put(argc, argv, cmd):
	auto const color = env::opt::get(key::color, true);
	auto const serial = env::opt::get(key::serial, false);
	auto const tools = env::opt::get(key::tools, _TOOLS);
	auto const clean = std::empty(env::opt::arguments);

	// Initialize from tools
	if (not empty(tools))
	{
		ifstream in { tools };
		in >> env::opt::get;
		if (in.fail())
		{
			cerr << "Failed to read" << tools;
		}
	}

	// Use tool set
	if (empty(tests))
	{
		tests = fmt::split(env::opt::get("TESTS"));
	}

	// Map test names to error buffers' string stream
	map<fmt::string::view, fmt::string::stream> context;

	if (empty(tests))
	{
		view const program = env::opt::program;
		env::dev::dump dump; // output cache

		// Parse this programs symbol table
		for (auto line : dump.dyn(program))
		{
			// Separate lines by white space
			for (auto name : fmt::split(line))
			{
				// Match those with prefix
				if (name.starts_with(prefix))
				{
					// Symbol must exist
					auto call = sym(name);
					if (nullptr != call)
					{
						context.emplace(name, { });
					}
				}
			}
		}
	}
	else // copy
	{
		for (auto word : tests)
		{
			auto const name = prefix + word;
			auto const call = sym(name);
			if (nullptr == call)
			{
				cerr << "Cannot find " << name << " in " << program;
			}
			else
			{
				context.emplace(name, { });
			}
		}
	}

	// Print the unit tests and quit
	if (env::opt::get(key::print, false))
	{
		for (auto const & [name, error] : context)
		{
			cerr << name << eol;
		}
		return EXIT_SUCCESS;
	}

	bool const missing = clean and empty(context);
	// Print the help menu and quit if missing
	if (env::opt::get(key::help, missing)
	{
		if (missing)
		{
			if (color) cerr << fg_yellow;

			cerr << "No tests were found" << eol;

			if (color) cerr << fg_off;
		}

		cerr 
			<< "Unit tests are found in order:"
			<< eol << tab
			<< "1. Free command line arguments"
			<< eol << tab
			<< "2. The TESTS environment variable"
			<< eol << tab
			<< "3. The TESTS variable in " _TOOLS
			<< eol << tab
			<< "4. The dump symbols for " << prefix << "*"
			<< eol
			<< "Commands for unit test runner:"
			<< eol;

		for (auto const & item : cmd)
		{
			cerr
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

	// Run all the selected unit tests 
	{
		std::set<std::future<void>> threads;

		// Run tests either in serial or parallel
		for (auto & [name, error] : constext)
		{
			auto buf = error.rdbuf();
			if (not serial)
			{
				threads.emplace(async(runner, name, buf));
			}
			else
			{
				runner(name, buf);
			}
		}

		// Wait on test completion
		for (auto & job : threads)
		{
			job.wait();
		}
	}

	if (color)
	{
		cerr << fg_yellow;
	}

	signed long int counter = 0;
	for (auto & [name, error] : context)
	{
		auto str = error.str();

		if (not empty(str))
		{
			while (getline(error, str))
			{
				cerr << name << tab << str << eol;
				++counter;
			}

			if (buf.fail())
			{
				cerr << "Failed to read " << name;
			}
		}
	}

	if (color)
	{
		cerr << (counter ? fg_cyan : fg_magenta);
	}

	cerr << "There are " << counter << " errors" << eol;

	if (color)
	{
		cerr << reset;
	}

	return counter ? EXIT_SUCCESS : EXIT_FAILURE ;
}

