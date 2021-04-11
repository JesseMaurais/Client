// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "test.hpp"
#include "arg.hpp"
#include "cmd.hpp"
#include "str.hpp"
#include "type.hpp"
#include "char.hpp"
#include "sym.hpp"
#include "dev.hpp"
#include "sig.hpp"
#include "err.hpp"
#include <iostream>
#include <fstream>
#include <future>
#include <vector>
#include <set>
#include <map>

#ifndef _CONFIG
# define _CONFIG ".ini"
#endif

#ifndef _MSC_VER
#ifdef test_unit
test_unit(err)
{
	assert(true == true);
	assert(true != false);
	assert(true and not false);
	except(throw "Holy Cow!");
}
#endif
#endif

namespace
{
	void runner(fmt::string::view name, fmt::string::buf::ptr buf, bool host)
	{
		auto back = sys::out().rdbuf();
		try
		{
			sys::out().rdbuf(buf);
			if (host)
			{
				auto call = sys::sym<void()>(name);
				if (nullptr == call)
				{
					sys::out() << name << " is missing";
				}
				else
				{
					call();
				}
			}
			else
			{
				struct env::shell sh;
				auto const image = env::opt::arg();
				fmt::string::view::vector args { image, "-o", "-q", name };
				for (auto line : sh.run(args))
				{
					sys::out() << line << fmt::eol;
				}

				if (0 < sh.status)
				{
					auto const text = sys::sig::to_string(sh.status);
					sys::out() << "Signal: " << text << fmt::eol;
				}
				else
				if (sh.status < 0)
				{
					sys::out() << "Failed: " << sh.status << fmt::eol;
				}
			}
		}
		catch (std::exception const& error)
		{
			sys::out() << error.what() << fmt::eol;
		}
		catch (...)
		{
			sys::out() << "Unknown" << fmt::eol;
		}
		sys::out().rdbuf(back);
	}
}

int main(int argc, char** argv)
{
	// Default options file
	fmt::string const config = _CONFIG;

	// Command line words
	struct
	{
		env::opt::name const
			tests = fmt::put("TESTS"),
			color = fmt::put("color"),
			async = fmt::put("async"),
			tools = fmt::put("tools"),
			print = fmt::put("print"),
			quiet = fmt::put("quiet"),
			host  = fmt::put("host"),
			help  = fmt::put("help");
	} arg;

	// Command line details
	env::opt::command::vector cmd
	{
		{ 0, "h", fmt::get(arg.help), "Print command line usage then quit" },
		{ 0, "p", fmt::get(arg.print), "Print all source tests then quit" },
		{ 0, "q", fmt::get(arg.quiet), "Only print error messages" },
		{ 0, "c", fmt::get(arg.color), "Print using color codes" },
		{ 0, "a", fmt::get(arg.async), "Run tests asynchronously" },
		{ 1, "i", fmt::get(arg.tools), _CONFIG " is replaced with argument" },
		{ 0, "o", fmt::get(arg.host), "Host tests in this process" },
	};

	// Command line parsing
	auto tests = env::opt::put(argc, argv, cmd);

	// Command line options
	auto const host  = env::opt::get(arg.host, false);
	auto const color = env::opt::get(arg.color, not host);
	auto const quiet = env::opt::get(arg.quiet, false);
	auto const async = env::opt::get(arg.async, false);
	auto const tools = env::opt::get(arg.tools, config);
	auto const clean = std::empty(env::opt::arguments());

	// Initialize from tools
	if (not std::empty(tools))
	{
		auto const path = fmt::to_string(tools);
		std::ifstream in { path };
		if (in)
		{
			in >> env::opt::get;
		}
		else
		{
			std::cerr << "Failed to open " << path << fmt::eol;
		}
	}

	// Default test options
	if (std::empty(tests))
	{
		static const auto list = env::opt::get(arg.tests);
		for (const auto test : fmt::split(list, ";"))
		{
			tests.emplace_back(test);
		}
	}

	// Map test names to error buffers' string stream
	std::map<fmt::string, fmt::string::stream> context;
	fmt::string::view const prefix = "test_";
	auto const program = env::opt::program();

	if (std::empty(tests))
	{
		// Parse this programs symbol table
		for (auto const& line : env::dev().dyn(argv[0]))
		{
			// Separate lines by white space
			for (auto const name : fmt::split(line))
			{
				// Match those with prefix
				if (name.starts_with(prefix))
				{
					// Symbol must exist
					auto const call = sys::sym<void()>(name);
					if (nullptr != call)
					{
						verify(context[name].str().empty());
					}
				}
			}
		}
	}
	else // copy
	{
		for (fmt::string const name : tests)
		{
			auto const call = sys::sym<void()>(name);
			if (nullptr == call)
			{
				std::cout << "Cannot find " << name << " in " << program << fmt::eol;
			}
			else
			{
				verify(context[name].str().empty());
			}
		}
	}

	// Print the unit tests and quit
	if (env::opt::get(arg.print, false))
	{
		for (auto const& [name, error] : context)
		{
			std::cout << name << fmt::eol;
		}
		return EXIT_SUCCESS;
	}

	bool const missing = clean and std::empty(context);
	// Print the help menu and quit if missing
	if (env::opt::get(arg.help, missing))
	{
		if (missing)
		{
			if (color) std::cout << fmt::io::fg_yellow;

			std::cout << "No tests were found" << fmt::eol;

			if (color) std::cout << fmt::io::fg_off;
		}

		std::cout 
			<< "Unit tests are found in order:"
			<< fmt::eol << fmt::tab
			<< "1. Free command line arguments"
			<< fmt::eol << fmt::tab
			<< "2. The TESTS environment variable"
			<< fmt::eol << fmt::tab
			<< "3. The TESTS variable in " _CONFIG
			<< fmt::eol << fmt::tab
			<< "4. The dump symbols for " << prefix << "*"
			<< fmt::eol
			<< "Commands for unit test runner:"
			<< fmt::eol;

		for (auto const & item : cmd)
		{
			std::cout
				<< fmt::tab
				<< env::opt::dash << item.dash
				<< ' '
				<< env::opt::dual << item.name
				<< fmt::tab
				<< item.text
				<< fmt::eol;
		}
		return EXIT_SUCCESS;
	}

	// Run all the selected unit tests 
	{
		std::vector<std::future<void>> threads;

		// Run tests either in serial or parallel
		for (auto const& [name, error] : context)
		{
			auto buf = error.rdbuf();
			if (async)
			{
				threads.emplace_back
				(
					std::async(std::launch::async, runner, name, buf, host)
				);
			}
			else
			{
				runner(name, buf, host);
			}
		}

		// Wait on test completion
		for (auto& job : threads)
		{
			job.wait();
		}
	}

	std::size_t counter = 0;
	for (auto& [name, error] : context)
	{
		if (auto str = error.str(); not std::empty(str))
		{
			if (color)
			{
				std::cout << fmt::io::fg_yellow;
			}

			while (std::getline(error, str))
			{
				if (not quiet)
				{
					std::cout << name << fmt::tab << str << fmt::eol;
				}
				else
				{
					std::cout << str << fmt::eol;
				}
				++ counter;
			}
		}
		else
		{
			if (color)
			{
				std::cout << fmt::io::fg_green;
			}

			if (not quiet)
			{
				std::cout << name << fmt::tab << "ok" << fmt::eol;
			}
		}
	}

	if (color)
	{
		std::cout << (0 < counter ? fmt::io::fg_magenta : fmt::io::fg_cyan);
	}

	if (not quiet)
	{
		std::cout << "There are " << counter << " errors" << fmt::eol;
	}

	if (color)
	{
		std::cout << fmt::io::reset;
	}

	std::cout << std::flush;

	return 0 < counter ? EXIT_FAILURE : EXIT_SUCCESS;
}

