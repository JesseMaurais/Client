// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "exe.hpp"
#include "opt.hpp"
#include "arg.hpp"
#include "env.hpp"
#include "usr.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "io.hpp"

namespace env::exe
{
	static thread_local fmt::string::set buf;

	void clear()
	{
		buf.clear();
	}

	fmt::vector get(fmt::input in, char end, int count)
	{
		fmt::vector lines;
		try
		{
			std::string line;
			while (count-- and std::getline(in, line, end))
			{
				auto [it, unique] = buf.emplace(std::move(line));
				(void) unique; // duplicates are okay
				lines.emplace_back(it->data(), it->size());
			}
		}
		catch (std::exception &error)
		{
			sys::err(here, error.what());
		}
		return lines;
	}

	fmt::vector get(fmt::span args)
	{
		fmt::string command;
		for (auto arg : args)
		{
			const bool spaces = fmt::any_of(arg);
			if (spaces) command += fmt::tag::quote;
			command += arg;
			if (spaces) command += fmt::tag::quote;
			command += " ";
		}
		fmt::istream in = env::file::open(command, env::file::ex);
		const auto lines = get(in);
		in.file.reset();
		return lines;
	}

	fmt::vector get(fmt::init args)
	{
		return get(fwd::to_span(args));
	}

	fmt::vector echo(fmt::view line)
	{
		return get({ "echo", line });
	}

	fmt::vector list(fmt::view name)
	{
		return get
		(
		#ifdef _WIN32
			{ "dir", "/b", name }
		#else
			{ "ls", name }
		#endif
		);
	}

	fmt::vector copy(fmt::view path)
	{
		return get
		(
		#ifdef _WIN32
			{ "type", path }
		#else
			{ "cat", path }
		#endif
		);
	}

	fmt::vector find(fmt::view pattern, fmt::view directory)
	{
		#ifdef _WIN32
		{
			return list(fmt::dir::join({directory, pattern}));
		}
		#else
		{
			return get({ "find", directory, "-type", "f", "-name", pattern });
		}
		#endif
	}

	fmt::vector which(fmt::view name)
	{
		return get
		(
		#ifdef _WIN32
			{ "where", name }
		#else
			{ "which", "-a", name }
		#endif
		);
	}

	fmt::vector start(fmt::view path)
	{
		#ifdef _WIN32
		{
			return get({ "start", "/d", path });
		}
		#else
		{
			const fmt::pair test [] =
			{
				{ "xfce", "exo-open" },
				{ "gnome", "gnome-open" },
				{ "kde", "kde-open" },
				{ "", "xdg-open" },
			};

			for (auto [session, program] : test)
			{
				if (session.empty() or desktop(session))
				{
					if (auto path = which(program); path.empty())
					{
						continue;
					}

					return get({ program, path });
				}
			}
			return fmt::vector();
		}
		#endif
	}

	fmt::vector imports(fmt::view path)
	{
		return get
		(
		#ifdef _WIN32
			{ "dumpbin", "-nologo", "-imports", path }
		#else
			{ "objdump", "-t", path }
		#endif
		);
	}

	fmt::vector exports(fmt::view path)
	{
		return get
		(
		#ifdef _WIN32
			{ "dumpbin", "-nologo", "-exports", path }
		#else
			{ "objdump", "-T", path }
		#endif
		);
	}

	bool desktop(fmt::view name)
	{
		const auto current = env::usr::current_desktop();
		const auto lower = fmt::to_lower(current);
		const auto find = fmt::to_lower(name);
		return lower.find(name) != fmt::npos;
	}

	static fmt::vector pick()
	{
		constexpr auto zenity = "zenity", qarma = "qarma";

		if (desktop("KDE") or desktop("LXQT"))
		{
			return { qarma, zenity };
		}
		else // GNOME or XFCE, etc
		{
			return { zenity, qarma };
		}
	}

	static fmt::view pick(fmt::view value)
	{
		using namespace std::literals;
		static fmt::pair entry { "Program Options", "DIALOG" };
		return env::opt::get(entry, value);
	}

	fmt::vector dialog(fmt::param par)
	{
		// Look for any desktop utility program
		fmt::view program;
		static const auto session = pick();
		for (auto test : session)
		{
			if (auto path = which(test); not path.empty())
			{
				program = path.front();
				break;
			}
		}
		program = pick(program);
		// Append the command line
		fmt::vector command;
		fmt::cache cache;
		command.push_back(program);
		for (auto pair : par)
		{
			auto [it, unique] = cache.emplace(fmt::opt::join(pair));
			#ifdef assert
			assert(unique);
			#endif
			command.emplace_back(it->data(), it->size());
		}
		return get(command);
	}


	fmt::vector select(fmt::view path, fmt::view mode)
	{
		fmt::edges command {{ "file-selection", fmt::tag::empty }};
		if (not path.empty())
		{
			command.emplace_back("filename", path);
		}
		if (fwd::any_of(fmt::init{"multiple", "directory", "save"}, fwd::equal_to(mode)))
		{
			command.emplace_back(mode, fmt::tag::empty);
		}
		return dialog(command);
	}

	fmt::vector show(fmt::view text, fmt::view type)
	{
		fmt::edges command {{ type, fmt::tag::empty }};
		if (not text.empty())
		{
			command.emplace_back("text", text);
		}
		return dialog(command);
	}

	fmt::vector enter(fmt::view start, fmt::view label, bool hide)
	{
		fmt::edges command {{ "entry-text", start }};
		if (not label.empty())
		{
			command.emplace_back("text", label);
		}
		if (hide)
		{
			command.emplace_back("hide-text", fmt::tag::empty);
		}
		return dialog(command);
	}

	fmt::vector text(fmt::view path, fmt::view check, fmt::view font, fmt::view type)
	{
		fmt::edges command {{ "text-info", fmt::tag::empty }};
		if (type == "html")
		{
			command.emplace_back(type, fmt::tag::empty);
			command.emplace_back("url", path);
		}
		else
		{
			if (type == "editable")
			{
				command.emplace_back(type, fmt::tag::empty);
			}
			command.emplace_back("filename", path);
		}
		if (not font.empty())
		{
			command.emplace_back("font", font);
		}
		if (not check.empty())
		{
			command.emplace_back("checkbox", check);
		}
		return dialog(command);

	}

	fmt::vector form(fmt::param add, fmt::view text, fmt::view title)
	{
		fmt::edges command {{ "forms", fmt::tag::empty }};
		if (not text.empty())
		{
			command.emplace_back("text", text);
		}
		if (not title.empty())
		{
			command.emplace_back("title", title);
		}
		fmt::string prefix { "add-" };
		for (auto ctl : add)
		{
			fmt::string copy = ctl.second;
			auto key = prefix + copy;
			command.emplace_back(key, ctl.first);
		}
		return dialog(command);
	}

	fmt::vector notify(fmt::view text, fmt::view icon)
	{
		fmt::edges command {{ "notification", fmt::tag::empty }};
		if (not text.empty())
		{
			command.emplace_back("text", text);
		}
		if (not icon.empty())
		{
			command.emplace_back("icon", icon);
		}
		return dialog(command);
	}

	fmt::vector calendar(fmt::view text, fmt::view format, int day, int month, int year)
	{
		fmt::edges command {{ "calendar", fmt::tag::empty }};
		if (not text.empty())
		{
			command.emplace_back("text", text);
		}
		if (not format.empty())
		{
			command.emplace_back("format", format);
		}
		if (auto s = fmt::to_string(day); 0 < day)
		{
			command.emplace_back("day", s);
		}
		if (auto s = fmt::to_string(month);0 < month)
		{
			command.emplace_back("month", s);
		}
		if (auto s = fmt::to_string(year); 0 < year)
		{
			command.emplace_back("year", s);
		}
		return dialog(command);
	}

	fmt::vector color(fmt::view start, bool palette)
	{
		fmt::edges command {{"color-selection", fmt::tag::empty }};
		if (not start.empty())
		{
			command.emplace_back("color", start);
		}
		if (palette)
		{
			command.emplace_back("show-palette", fmt::tag::empty);
		}
		return dialog(command);
	}
}

#ifdef test_unit
test_unit(cmd)
{
	const auto list = env::exe::list(env::pwd());
	assert(not list.empty());
	const auto copy = env::exe::copy(__FILE__);
	assert(not copy.empty());
	// Copy range starts at 0, file numbering at 1
	assert(copy[__LINE__-1].find("Recursive find me text") != fmt::npos);
}
test_unit(echo)
{
	fmt::view user = env::got("ComSpec")
		? "%UserName%" : "$USER";

	const auto echo = env::exe::echo(user);
	assert(not echo.empty());
	const auto name = echo.front();
	assert(user != name);
}
#endif