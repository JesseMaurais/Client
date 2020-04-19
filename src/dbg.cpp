// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "esc.hpp"
#include "ptr.hpp"
#include "type.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cassert>
#include <regex>
#include <map>

namespace
{
	struct eat : unique, std::stringstream
	{
		eat(std::ostream& o = std::cerr) : out(o)
		{
			buf = out.rdbuf();
			out.rdbuf(rdbuf());
		}

		~eat()
		{
			out.rdbuf(buf);
		}

	private:

		std::ostream& out;
		std::streambuf* buf;
	};
}

namespace dbg
{
	using namespace fmt;

	namespace
	{
		size_type max_size = 0;

		auto& registry()
		{
			static std::map<test*, string> instance;
			return instance;
		}

		std::ostream& stream(fmt::string::view pattern)
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
		max_size = std::max(max_size, it->second.size());
		assert(unique);
	}

	test::~test()
	{
		registry().erase(this);
	}

	void fail::run()
	{
		eat err;
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
				string const indent(max_size - name.size(), ' ');
				out << faint << name << intense_off << indent;

				eat err;
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
