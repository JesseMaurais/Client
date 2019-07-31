// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "esc.hpp"

#include <stdexcept>
#include <iostream>
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
		std::stringstream ss;
		auto const os = std::cerr.rdbuf();
		std::cerr.rdbuf(ss.rdbuf());
		die();
		const auto s = ss.str();
		std::cerr.rdbuf(os);

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

		using namespace io;
		constexpr auto eol = '\n';
		auto const& tests = registry();
		auto& out = std::cout;

		int errors = 0;
		std::regex pattern(expression);
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
			auto const what = except.what();
			out << fg_red << '\t' << what << fg_off << eol;
		}
		catch (...)
		{
			++errors;
			constexpr auto what = "exception";
			out << fg_red << '\t' << what << fg_off << eol;
		}

		auto fg_color = errors ? fg_yellow : fg_blue;
		out << fg_color << errors << " errors detected." << fg_off << eol;

		out << reset << eol;
		return errors;
	}
}

