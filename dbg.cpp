// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "esc.hpp"
#include "ios.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <string>
#include <regex>
#include <map>

namespace
{
	using map = std::map<dbg::test*, std::string>;
	std::string::size_type max_size = 0;

	auto& registry()
	{
		static map instance;
		return instance;
	}
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
		io::eat_streambuf eat(std::cerr);
		try
		{
			die();
		}
		catch (std::exception const& except)
		{
			std::cerr << except.what();
		}

		auto const s = eat.str();
		if (empty(s))
		{
			throw std::runtime_error("Failing test succeeded");
		}
	}

	int run(char const *expression)
	{
		if (nullptr == expression)
		{
			expression = "^[^_](.*?)";
		}

		using namespace fmt;
		constexpr auto eol = '\n';
		auto const& tests = registry();
		auto& out = std::cout;

		int nerr = 0;
		std::regex pattern(expression);
		for (auto const& [that, name] : tests)
		{
			if (std::regex_match(name, pattern)) try
			{
				std::string const indent(max_size - name.size(), ' ');
				out << faint << name << intense_off << indent;
				io::eat_streambuf eat(std::cerr);
				that->run();

				auto s = eat.str();
				if (not empty(s))
				{
					constexpr auto npos = std::string::npos;
					auto pos = npos;
					while ((pos = s.find("\n")) != npos)
					{
						s.replace(pos, 1, " ");
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
		out << fg_color << nerr << " errors detected." << fg_off << eol;

		out << reset << eol;
		return nerr;
	}
}

