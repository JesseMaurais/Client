// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "ctl.hpp"
#include "ios.hpp"
#include "str.hpp"
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <regex>
#include <map>

namespace dbg
{
	using namespace fmt;

	namespace
	{
		string::size_type max_size = 0;

		auto& registry()
		{
			static std::map<test*, string> instance;
			return instance;
		}

		std::ostream& stream(fmt::string_view pattern)
		{
			if (pattern.front() == '_')
			{
				static std::stringstream sink;
				return sink;
			}
			return std::cout;
		}
	}

	test::test(char const *name)
	{
		auto const [it, unique] = registry().emplace(this, name);
		max_size = std::max(max_size, size(it->second));
		assert(unique);
	}

	test::~test()
	{
		registry().erase(this);
	}

	void fail::run()
	{
		io::eat err;
		try
		{
			die();
		}
		catch (std::exception const& except)
		{
			err << except.what();
		}

		auto const s = err.str();
		if (empty(s))
		{
			throw std::runtime_error("Failing test succeeded");
		}
	}

	int run(char const* expression)
	{
		if (nullptr == expression)
		{
			expression = "^[^_](.*?)";
		}
		auto & out = stream(expression);

		int nerr = 0;
		std::regex pattern(expression);
		for (auto const& [that, name] : registry())
		{
			if (std::regex_match(name, pattern)) try
			{
				string const indent(max_size - size(name), ' ');
				out << faint << name << intense_off << indent;

				io::eat err;
				that->run();
				auto s = err.str();
				if (not empty(s))
				{
					auto pos = npos;
					while (npos != (pos = s.find("\n")))
					{
						s.replace(pos, 1, ";");
					}
					throw std::runtime_error(s);
				}
				out << fg_green << "\tok" << fg_off << eol;
			}
			catch (std::exception const& except)
			{
				++ nerr;
				auto const what = except.what();
				out << fg_red << '\t' << what << fg_off << eol;
			}
		}

		auto const fg_color = nerr ? fg_yellow : fg_blue;
		out << fg_color << nerr << " errors detected." << reset << eol;
		out << std::endl;
		return nerr;
	}
}

