// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "exe.hpp"
#include "env.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "io.hpp"

namespace env::exe
{
	fmt::string::set& cache()
	{
		thread_local fmt::string::set buf;
		return buf;
	}

	fmt::vector get(fmt::input in, char end, int count)
	{
		fmt::vector lines;
		try
		{
			std::string line;
			while (count-- and std::getline(in, line, end))
			{
				lines.emplace_back(cache().emplace(std::move(line)));
			}
		}
		catch (std::exception &error)
		{
			sys::err(here, error.what());
		}
		return lines;
	}

	fmt::vector run(fmt::span args)
	{
		fmt::string command;
		for (auto arg : args)
		{
			const bool spaces = fmt::any_of(arg);
			constexpr auto quote = "\"";
			if (spaces) command += quote;
			command += arg;
			if (spaces) command += quote;
		}
		fmt::istream in = env::file::open(command, env::file::ex);
		const auto lines = get(in);
		in.file.reset();
		return lines;
	}

	fmt::vector run(fmt::init args)
	{
		return run(fwd::to_span(args));
	}

	fmt::vector echo(fmt::view line)
	{
		return run({ "echo", line });
	}

	fmt::vector list(fmt::view name)
	{
		return run
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
		return run
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
			return run({ "find", directory, "-type", "f", "-name", pattern });
		}
		#endif
	}

	fmt::vector which(fmt::view name)
	{
		return run
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
			return run({ "start", "/d", path });
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

					return run({ program, path });
				}
			}
			return { 0, 0, nullptr };
		}
		#endif
	}

	fmt::vector imports(fmt::view path)
	{
		return run
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
		return run
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

	static fmt::vector envpick()
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

	static fmt::view optpick(fmt::view value)
	{
		using namespace std::literals;
		static auto entry = std::make_pair("Runtime Options"sv, "DIALOG"sv);
		return env::opt::get(entry, value);
	}

	fmt::vector dialog(fmt::span args)
	{
		// Look for any desktop utility program
		fmt::view program;
		static auto const session = envpick();
		for (auto test : session)
		{
			if (auto path = which(test); not path.empty())
			{
				//program = path;
				break;
			}
		}
		program = optpick(program);
		// Append the command line
		fmt::vector command;
		command.push_back(program);
		for (auto arg : args)
		{
			command.push_back(arg);
		}
		return run(command);
	}


	fmt::vector select(fmt::view path, mode mask)
	{
		fmt::vector command { "--file-selection" };

		if (not path.empty())
		{
			command.emplace_back("--filename", path);
		}
		if (mask == mode::many)
		{
			command.emplace_back("--multiple");
		}
		if (mask == mode::dir)
		{
			command.emplace_back("--directory");
		}
		if (mask == mode::save)
		{
			command.emplace_back("--save");
		}

		return dialog(command);
	}

	static auto message(msg type)
	{
		switch (type)
		{
		default:
		case msg::error:
			return "--error";
		case msg::info:
			return "--info";
		case msg::query:
			return "--question";
		case msg::warn:
			return "--warn";
		}
	};

	fmt::vector show(fmt::view text, msg type)
	{
		fmt::vector command { message(type) };
		if (not text.empty())
		{
			command.emplace_back("--text", text);
		}
		return dialog(command);
	}

	fmt::vector enter(fmt::view start, fmt::view label, bool hide)
	{
		fmt::vector command {{ "--entry-text", start }};
		if (not label.empty())
		{
			command.emplace_back("--text", label);
		}
		if (hide)
		{
			command.emplace_back("--hide-text");
		}
		return dialog(command);
	}

	fmt::vector text(fmt::view path, fmt::view check, fmt::view font, txt type)
	{
		fmt::vector command { "--text-info" };
		if (type == txt::html)
		{
			command.emplace_back("--html");
			command.emplace_back("--url", path);
		}
		else
		{
			if (type == txt::edit)
			{
				command.emplace_back("--editable");
			}
			command.emplace_back("--filename", path);
		}
		if (not font.empty())
		{
			command.emplace_back("--font", font);
		}
		if (not empty(check))
		{
			command.emplace_back("--checkbox", check);
		}
		return dialog(command);

	}

	fmt::vector form(controls add, fmt::view text, fmt::view title)
	{
		fmt::vector command { "--forms" };
		if (not text.empty())
		{
			command.emplace_back("--text", text);
		}
		if (not title.empty())
		{
			command.emplace_back("--text", title);
		}
		fmt::string prefix { "--add-" };
		for (auto ctl : add)
		{
			auto key = prefix + fmt::to_string(ctl.second);
			command.emplace_back(key, ctl.first);
		}
		return dialog(command);
	}

	fmt::vector notify(fmt::view text, fmt::view icon)
	{
		fmt::vector command { "--notification" };
		if (not text.empty())
		{
			command.emplace_back("--text", text);
		}
		if (not icon.empty())
		{
			command.emplace_back("--icon", icon);
		}
		return dialog(command);
	}

	fmt::vector calendar(fmt::view text, fmt::view format, int day, int month, int year)
	{
		fmt::vector command { "--calendar" };
		if (not text.empty())
		{
			command.emplace_back("--text", text);
		}
		if (not format.empty())
		{
			command.emplace_back("--format", format);
		}
		if (0 < day)
		{
			command.emplace_back("--day", fmt::to_string(day));
		}
		if (0 < month)
		{
			command.emplace_back("--month", fmt::to_string(month));
		}
		if (0 < year)
		{
			command.emplace_back("--year", fmt::to_string(year));
		}
		return dialog(command);
	}

	fmt::vector color(fmt::view start, bool palette)
	{
		fmt::vector command { "--color-selection" };
		if (not start.empty())
		{
			command.emplace_back("--color", start);
		}
		if (palette)
		{
			command.emplace_back("--show-palette");
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