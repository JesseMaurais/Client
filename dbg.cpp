// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <csignal>
#include <string>
#include <regex>
#include <map>

#ifdef _WIN32
# include "win.hpp"
#endif
#include "dbg.hpp"
#include "esc.hpp"

namespace
{
	//TEST(dumb_assert) { ASSERT(false); };
	//TEST(dumb_except) { throw std::runtime_error("Holy Cow!"); };
	//TEST(_dumb) { throw "You should not run hidden unit tests."; }; 

	using map = std::map<dbg::test*, std::string>;
	std::string::size_type max_size = 0;

	auto& registry()
	{
		#ifdef _WIN32
		{
			static auto quit = sys::win::sig::quit(SIGINT);
		}
		#endif
		static map instance;
		return instance;
	}
}

namespace sys
{
	bool debug =
	#ifdef NDEBUG
		false;
	#else
		true;
	#endif
}

namespace dbg
{
	test::test(char const *name)
	{
		auto const [it, unique] = registry().emplace(this, name);
		assert(unique);
		auto const size = it->second.size();
		max_size = std::max(max_size, size);
	}

	test::~test()
	{
		registry().erase(this);
	}

	void fail::run()
	{
		std::stringstream ss;
		auto const os = std::cerr.rdbuf();
		std::cerr.rdbuf(ss.rdbuf());
		die();
		const auto s = ss.str();
		std::cerr.rdbuf(os);
		if (not empty(s))
		{
			throw std::runtime_error(s);
		}
	}

	int run(char const *expression)
	{
		if (not expression)
		{
			expression = "^[^_](.*?)";
		}
		std::regex pattern(expression);

		using namespace io;
		auto& out = std::cerr;
		constexpr auto eol = '\n';
		auto const& tests = registry();

		unsigned errors = 0;
		for (auto const& [that, name] : tests) try
		{
			if (std::regex_match(name, pattern))
			{
				std::string const indent(max_size - name.size(), ' ');
				out << faint << name << intense_off << indent;
				that->run();
				out << fg_green << "\tok" << fg_off << eol;
			}
		}
		catch (std::exception const& except)
		{
			++errors;
			out << fg_red << "\tthrown: " << except.what() << fg_off << eol;
		}
		catch (char const* message)
		{
			++errors;
			out << fg_red << '\t' << message << fg_off << eol;
		}
		catch (...)
		{
			++errors;
			out << fg_red << "\tUnknown exception" << fg_off << eol;
		}

		auto fg_color = errors ? fg_yellow : fg_blue;
		out << fg_color << errors << " errors detected." << fg_off << eol;

		out << reset << eol;
		return errors;
	}
}

