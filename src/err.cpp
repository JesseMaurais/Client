// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "env.hpp"
#include "arg.hpp"
#include "exe.hpp"
#include "type.hpp"
#include "char.hpp"
#include "sym.hpp"
#include "sync.hpp"
#include <iostream>
#include <fstream>
#include <future>
#include <vector>
#include <set>
#include <map>

namespace fmt
{
	bool debug =
	#ifdef NDEBUG
		false;
	#else
		true;
	#endif

	thread_local buffer local_buf;

	std::ostream& out()
	{
		return local_buf;
	}

	std::ostream& put(std::ostream& buf)
	{
		string line;
		while (std::getline(local_buf, line))
		{
			buf << line << std::endl;
		}
		return buf;
	}
}

namespace
{
	void runner(fmt::view name, fmt::string::buf::ptr buf, bool host)
	{
		auto back = fmt::out().rdbuf();
		try
		{
			fmt::out().rdbuf(buf);
			if (host)
			{
				auto call = sys::sym<void()>(name);
				if (nullptr == call)
				{
					fmt::out() << name << " is missing";
				}
				else
				{
					call();
				}
			}
			else
			{
				const auto image = env::opt::arg();
				fmt::vector args { image, "-o", "-q", name };
				for (auto line : env::exe::get(args))
				{
					fmt::out() << line << fmt::tag:: eol;
				}
			}
		}
		catch (std::exception const& error)
		{
			fmt::out() << error.what() << fmt::tag::eol;
		}
		catch (...)
		{
			fmt::out() << "Unknown" << fmt::tag::eol;
		}
		fmt::out().rdbuf(back);
	}
}

int main(int argc, char** argv)
{
	#ifndef _TOOLS
	#define _TOOLS "Tools.ini"
	#endif

	// Default options file
	const fmt::string config = _TOOLS;

	// Command line words
	struct
	{
		fmt::view
			tests = "TESTS",
			color = "color",
			async = "async",
			tools = "tools",
			print = "print",
			quiet = "quiet",
			host  = "host",
			help  = "help";
	} arg;

	// Command line details
	env::opt::cmd::vector cmd =
	{
		{ {}, 0, "h", arg.help, "Print command line usage then quit" },
		{ {}, 0, "p", arg.print, "Print all source tests then quit" },
		{ {}, 0, "q", arg.quiet, "Only print error messages" },
		{ {}, 0, "c", arg.color, "Print using color codes" },
		{ {}, 0, "a", arg.async, "Run tests asynchronously" },
		{ {}, 1, "t", arg.tools, _TOOLS " is replaced with argument" },
		{ {}, 0, "o", arg.host, "Host tests in this process" },
	};

	// Command line parsing
	auto tests = env::opt::parse(argc, argv, cmd);

	// Command line options
	const auto host  = env::opt::get(arg.host, false);
	const auto color = env::opt::get(arg.color, not host);
	const auto quiet = env::opt::get(arg.quiet, false);
	const auto async = env::opt::get(arg.async, false);
	const auto tools = env::opt::get(arg.tools, config);
	const auto clean = std::empty(env::opt::arguments());

	// Initialize from tools
	if (not std::empty(tools))
	{
		const auto path = fmt::to_string(tools);
		std::ifstream in { path };
		if (in)
		{
			in >> env::opt::get;
		}
		else
		{
			std::cerr << "Failed to open " << path << fmt::tag::eol;
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
	std::map<fmt::string, std::stringstream> context;
	const auto program = env::opt::program();
	constexpr fmt::view prefix = "test_";

	if (std::empty(tests))
	{
		// Parse this programs symbol table
		for (auto const& line : env::exe::exports(argv[0]))
		{
			// Separate lines by white space
			for (auto const name : fmt::split(line))
			{
				// Match those with prefix
				if (name.starts_with(prefix))
				{
					// Symbol must exist
					const auto call = sys::sym<void()>(name);
					if (nullptr != call)
					{
						assert(context[name].str().empty());
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
				std::cout << "Cannot find " << name << " in " << program << fmt::tag::eol;
			}
			else
			{
				assert(context[name].str().empty());
			}
		}
	}

	// Print the unit tests and quit
	if (env::opt::get(arg.print, false))
	{
		for (const auto& [name, error] : context)
		{
			std::cout << name << fmt::tag::eol;
		}
		return EXIT_SUCCESS;
	}

	const bool missing = clean and context.empty();
	// Print the help menu and quit if missing
	if (env::opt::get(arg.help, missing))
	{
		if (missing)
		{
			if (color) std::cout << fmt::io::fg_yellow;

			std::cout << "No tests were found" << fmt::tag::eol;

			if (color) std::cout << fmt::io::fg_off;
		}

		std::cout
			<< "Unit tests are found in order:"
			<< fmt::tag::eol << fmt::tag::tab
			<< "1. Free command line arguments"
			<< fmt::tag::eol << fmt::tag::tab
			<< "2. The TESTS environment variable"
			<< fmt::tag::eol << fmt::tag::tab
			<< "3. The TESTS variable in " _TOOLS
			<< fmt::tag::eol << fmt::tag::tab
			<< "4. The dump symbols for " << prefix << "*"
			<< fmt::tag::eol
			<< "Commands for unit test runner:"
			<< fmt::tag::eol;

		for (const auto& item : cmd)
		{
			std::cout
				<< fmt::tag::tab
				<< fmt::tag::dash << item.dash
				<< ' '
				<< fmt::tag::dual << item.name
				<< fmt::tag::tab
				<< env::text(item.text)
				<< fmt::tag::eol;
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
					std::cout << name << fmt::tag::tab << str << fmt::tag::eol;
				}
				else
				{
					std::cout << str << fmt::tag::eol;
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
				std::cout << name << fmt::tag::tab << "ok" << fmt::tag::eol;
			}
		}
	}

	if (color)
	{
		std::cout << (0 < counter ? fmt::io::fg_magenta : fmt::io::fg_cyan);
	}

	if (not quiet)
	{
		std::cout << "There are " << counter << " errors" << fmt::tag::eol;
	}

	if (color)
	{
		std::cout << fmt::io::reset;
	}

	std::cout << std::flush;

	return 0 < counter ? EXIT_FAILURE : EXIT_SUCCESS;
}

#ifdef TEST
TEST(err)
{
	#ifndef _MSC_VER
	assert(true == true);
	assert(true != false);
	assert(true and not false);
	except(throw "Holy Cow!");
	#endif
}
#endif
