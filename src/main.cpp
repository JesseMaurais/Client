#include "bug.hpp"
#include "arg.hpp"
#include "str.hpp"
#include "type.hpp"
#include "esc.hpp"
#include "sym.hpp"
#include "dev.hpp"
#include "sig.hpp"
#include "err.hpp"
#include <fstream>
#include <future>
#include <vector>
#include <set>
#include <map>

namespace
{
	fmt::string const prefix = "test_";

	void runner(fmt::string::view name, fmt::string::buf::ref buf)
	{
		auto& out = std::cerr;
		auto back = out.rdbuf();
		try
		{
			out.rdbuf(&buf);
			auto call = sys::sym<void()>(name);
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
			out << error.what() << fmt::eol;
		}
		catch (...)
		{
			out << "Unknown" << fmt::eol;
		}
		out.rdbuf(back);
	}

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
}

int main(int argc, char** argv)
{
	using namespace std;

	#ifndef _TOOLS
	# define _TOOLS "Tools.ini"
	#endif

	// Command line details
	std::vector<env::opt::command> cmd
	{
		{ 0, "h", key::help, "Print command line usage then quit" },
		{ 0, "p", key::print, "Print all source tests then quit" },
		{ 0, "c", key::color, "Print using color codes" },
		{ 0, "a", key::serial, "Run tests asynchronously" },
		{ 1, "t", key::tools, "Use instead of " _TOOLS },
	};

	// Command line parsing
	auto tests = env::opt::put(argc, argv, cmd);
	auto const color = env::opt::get(fmt::str::set(key::color), true);
	auto const serial = env::opt::get(fmt::str::set(key::serial), false);
	auto const tools = env::opt::get(fmt::str::set(key::tools), fmt::string::view(_TOOLS));
	auto const clean = std::empty(fmt::str::set(env::opt::arguments));

	// Initialize from tools
	if (not empty(tools))
	{
		ifstream in { tools.data() };
		in >> env::opt::get;
		if (in.fail())
		{
			cerr << "Failed to read" << tools;
		}
	}

	// Use tool set
	if (empty(tests))
	{
		const auto wd = fmt::str::set("TESTS");
		for (const auto test : fmt::split(env::opt::get(wd)))
		{
			tests.emplace_back(test);
		}
	}

	// Map test names to error buffers' string stream
	map<fmt::string::view, fmt::string::stream> context;
	fmt::string::view const program = env::opt::program;

	if (empty(tests))
	{
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
					auto call = sys::sym<void()>(name);
					if (nullptr != call)
					{
						context.emplace(name, sys::out);
					}
				}
			}
		}
	}
	else // copy
	{
		for (auto word : tests)
		{
			auto const name = prefix + fmt::to_string(word);
			auto const call = sys::sym<void()>(name);
			if (nullptr == call)
			{
				cerr << "Cannot find " << name << " in " << program;
			}
			else
			{
				context.emplace(name, sys::out);
			}
		}
	}

	// Print the unit tests and quit
	if (env::opt::get(fmt::str::set(key::print), false))
	{
		for (auto const & [name, error] : context)
		{
			cerr << name << fmt::eol;
		}
		return EXIT_SUCCESS;
	}

	bool const missing = clean and empty(context);
	// Print the help menu and quit if missing
	if (env::opt::get(fmt::str::set(key::help), missing))
	{
		if (missing)
		{
			if (color) cerr << fmt::fg_yellow;

			cerr << "No tests were found" << fmt::eol;

			if (color) cerr << fmt::fg_off;
		}

		cerr 
			<< "Unit tests are found in order:"
			<< fmt::eol << fmt::tab
			<< "1. Free command line arguments"
			<< fmt::eol << fmt::tab
			<< "2. The TESTS environment variable"
			<< fmt::eol << fmt::tab
			<< "3. The TESTS variable in " _TOOLS
			<< fmt::eol << fmt::tab
			<< "4. The dump symbols for " << prefix << "*"
			<< fmt::eol
			<< "Commands for unit test runner:"
			<< fmt::eol;

		for (auto const & item : cmd)
		{
			cerr
				<< fmt::tab
				<< env::opt::dash << item.dash
				<< fmt::tab
				<< env::opt::dual << item.name
				<< fmt::tab
				<< item.text
				<< fmt::eol;
		}
		return EXIT_SUCCESS;
	}

	// Run all the selected unit tests 
	{
		std::set<std::future<void>> threads;

		// Run tests either in serial or parallel
		for (auto & [name, error] : context)
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
		cerr << fmt::fg_yellow;
	}

	signed long int counter = 0;
	for (auto & [name, error] : context)
	{
		auto str = error.str();

		if (not empty(str))
		{
			while (getline(error, str))
			{
				cerr << name << fmt::tab << str << fmt::eol;
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
		cerr << (counter ? fmt::fg_cyan : fmt::fg_magenta);
	}

	cerr << "There are " << counter << " errors" << eol;

	if (color)
	{
		cerr << fmt::reset;
	}

	return counter ? EXIT_SUCCESS : EXIT_FAILURE ;
}

