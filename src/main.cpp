#include "bug.hpp"
#include "arg.hpp"
#include "esc.hpp"
#include "sym.hpp"
#include "dev.hpp"
#include <future>
#include <fstream>
#include <cassert> // overwrite assert

namespace
{
	fmt::string::view const prefix = "test_";

	auto sym(fmt::string::view name)
	{
		return sys::sym<void()>(name);
	}

	void runner(fmt::string::view name, fmt::string::buf::ref buf)
	{
		auto & out = sys::out();
		auto back = out.rdbuf();
		try
		{
			out.rdbuf(buf);
			auto call = sym(name);
			if (nullptr == call)
			{
				out << name << "is missing";
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
			help  = "help"  , 
			color = "color" , 
			async = "async" ,
			tools = "tools" ,
			show  = "show"  ;
	};

	#ifndef _TOOLS
	# define _TOOLS "Tools.ini"
	#endif

	// Command line details
	env::opt::commands const cmd
	{
	 { 0, "h", key::help, "Print command line usage then quit" },
	 { 0, "p", key::show, "Print all source tests then quit" },
	 { 0, "c", key::color, "Print using color codes" },
	 { 0, "a", key::serial, "Run tests asynchronously" },
	 { 1, "t", key::tools, "Use instead of " _TOOLS },
	};

	// Command line parsing
	auto const extra = env::opt::put(argc, argv, cmd):
	auto const color = env::opt::get(key::color, true);
	auto const serial = env::opt::get(key::serial, false);
	auto const tools = env::opt::get(key::tools, _TOOLS);
	auto const clear = std::empty(env::opt::arguments);

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
	if (empty(extra))
	{
		auto const tests = env::opt::get("TESTS");
		extra = fmt::split(tests, ",");
	}

	// Map test names to string buffer's backing error stream
	std::map<fmt::string::view, fmt::string::stream> streams;

	// Create streams
	if (empty(extra))
	{
		env::dev::dump dmp; // program symbol table
		view const program = env::opt::program;

		for (auto line : dmp.dyn(program))
		{
			// Separate by white space
			auto begin = line.begin();
			auto const end = line.end();
			for (auto it = skip(begin); it != end; it = skip(begin))
			{
				begin = next(it);
				view word(it, begin);
				if (npos != word.find(prefix))
				{
					auto call = sym(word);
					if (nullptr != call)
					{
						streams.emplace(word, { });
					}
				}
			}
		}
	}
	else // copy
	{
		for (view word : which)
		{
			auto const name = prefix + word;
			auto const call = sym(name);
			if (nullptr == call)
			{
				#ifdef trace
				trace("Cannot find", name, "in", program);
				#endif
			}
			else
			{
				streams.emplace(name, { });
			}
		}
	}

	// Print the test units and quit
	if (env::opt::get(key::show, false))
	{
		for (auto const & item : streams)
		{
			cerr << item.first << eol;
		}
		return EXIT_SUCCESS;
	}

	// Print the help menu and quit if missing
	if (env::opt::get(key::help, clear and empty(streams))
	{
		cerr 
			<< "Commands to run named unit tests"
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
		std::vector<std::future<void>> threads;

		// Spin off tests either in serial or parallel
		for (auto & [name, error] : streams)
		{
			auto buf = error.rdbuf();
			if (not serial)
			{
				threads.emplace_back(async(runner, name, buf));
			}
			else
			{
				runner(name, buf);
			}
		}
		// Wait for tests to complete
		if (not serial)
		{
			for (auto & job : threads)
			{
				job.wait();
			}
		}
	}

	if (color)
	{
		cerr << fg_yellow;
	}

	signed long int counter = 0;
	for (auto & [name, buf] : streams)
	{
		auto str = buf.str();

		if (not empty(str))
		{
			while (getline(buf, str))
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

