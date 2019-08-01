// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "esc.hpp"
#include "ios.hpp"
#include "str.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <chrono>
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

	int run(char const* expression)
	{
		if (nullptr == expression)
		{
			expression = "^[^_](.*?)";
		}

		int nerr = 0;
		std::regex pattern(expression);
		for (auto const& [that, name] : registry())
		{
			if (std::regex_match(name, pattern)) try
			{
				string const indent(max_size - size(name), ' ');
				std::cout << faint << name << intense_off << indent;
				io::eat_streambuf eat(std::cerr);

				that->run();

				auto s = eat.str();
				if (not empty(s))
				{
					auto pos = npos;
					while (npos != (pos = s.find("\n")))
					{
						s.replace(pos, 1, " ");
					}
					throw std::runtime_error(s);
				}
				std::cout << fg_green << "\tok" << fg_off << eol;
			}
			catch (std::exception const& except)
			{
				++ nerr;
				auto const what = except.what();
				std::cout << fg_red << '\t' << what << fg_off << eol;
			}
		}

		auto const fg_color = nerr ? fg_yellow : fg_blue;
		std::cout << fg_color << nerr << " errors detected." << reset << eol;
		std::cout << std::endl;
		return nerr;
	}
}

