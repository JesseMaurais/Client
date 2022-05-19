// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "cmd.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "io.hpp"

namespace env
{
	fmt::page shell::get(fmt::input in, char end, int count)
	{
		// Result in span at cache end
		const auto first = cache.size();
		auto second = first;
		try // process can crash
		{
			std::string line;
			while (count-- and std::getline(in, line, end))
			{
				buffer.append(line);
				buffer.push_back(end);
			}
			// Cache lines partition the buffer
			auto it = std::back_inserter(cache);
			for (auto part : fmt::split(buffer, end))
			{
				++it = part;
			}
			// One past the end
			second = cache.size();
		}
		// Put exception message into line
		catch (std::exception const& error)
		{
			buffer = error.what();
		}
		fmt::page result(first, second, &cache);
		return result;
	}

	fmt::page shell::run(fmt::span args)
	{
		fmt::string line;
		for (auto arg : args)
		{
			const bool spaces = fmt::any_of(arg);
			constexpr auto quote = "\"";
			if (spaces) line += quote;
			line += arg;
			if (spaces) line += quote;
		}
		fmt::istream in = env::file::open(line, env::file::ex);
		const auto lines = get(in);
		in.file.reset();
		return lines;
	}

	fmt::page shell::run(fmt::init args)
	{
		fmt::vector vec { args };
		return run(vec);
	}

	fmt::page shell::echo(fmt::view line)
	{
		return run({ "echo", line });
	}

	fmt::page shell::list(fmt::view name)
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

	fmt::page shell::copy(fmt::view path)
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

	fmt::page shell::find(fmt::view pattern, fmt::view directory)
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

	fmt::page shell::which(fmt::view name)
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

	fmt::page shell::start(fmt::view path)
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
			return { 0, 0, &cache };
		}
		#endif
	}

	fmt::page shell::imports(fmt::view path)
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

	fmt::page shell::exports(fmt::view path)
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

	bool shell::desktop(fmt::view name)
	{
		auto const lower = fmt::to_lower(name);
		auto const current = fmt::to_lower(env::usr::current_desktop());
		return current.find(lower) != fmt::npos;
	}

	static fmt::vector envpick()
	{
		constexpr auto zenity = "zenity", qarma = "qarma";

		if (shell::desktop("KDE") or shell::desktop("LXQT"))
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

	fmt::page shell::dialog(fmt::span args)
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

	static auto param(fmt::view key, fmt::view value)
	{
		fmt::view::vector pair { key, value };
		return fmt::join(pair, "=");
	}

	fmt::page shell::select(fmt::view path, mode mask)
	{
		fmt::vector command { "--file-selection" };

		if (not empty(path))
		{
			command.emplace_back(param("--filename", path));
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

	static auto message_type(shell::msg type)
	{
		switch (type)
		{
		default:
		case shell::msg::error:
			return "--error";
		case shell::msg::info:
			return "--info";
		case shell::msg::query:
			return "--question";
		case shell::msg::warn:
			return "--warn";
		}
	};

	fmt::page shell::message(fmt::view text, msg type)
	{
		fmt::vector command { message_type(type) };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}

		return dialog(command);
	}

	fmt::page shell::enter(fmt::view start, fmt::view label, bool hide)
	{
		fmt::vector command { param("--entry-text", start) };

		if (not empty(label))
		{
			command.emplace_back(param("--text", label));
		}
		if (hide)
		{
			command.emplace_back("--hide-text");
		}

		return dialog(command);
	}

	fmt::page shell::text(fmt::view path, fmt::view check, fmt::view font, txt type)
	{
		fmt::vector command { "--text-info" };

		if (type == txt::html)
		{
			command.emplace_back("--html");
			command.emplace_back(param("--url", path));
		}
		else
		{
			if (type == txt::edit)
			{
				command.emplace_back("--editable");
			}
			command.emplace_back(param("--filename", path));
		}
		if (not empty(font))
		{
			command.emplace_back(param("--font", font));
		}
		if (not empty(check))
		{
			command.emplace_back(param("--checkbox", check));
		}
		return dialog(command);

	}

	fmt::page shell::form(controls add, fmt::view text, fmt::view title)
	{
		fmt::vector command { "--forms" };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}
		if (not empty(title))
		{
			command.emplace_back(param("--text", title));
		}

		std::string const prefix("--add-");
		for (auto const& ctl : add)
		{
			auto const key = prefix + fmt::to_string(ctl.second);
			command.emplace_back(param(key, ctl.first));
		}

		return dialog(command);
	}

	fmt::page shell::notify(fmt::view text, fmt::view icon)
	{
		fmt::vector command { "--notification" };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}
		if (not empty(icon))
		{
			command.emplace_back(param("--icon", icon));
		}

		return dialog(command);
	}

	fmt::page shell::calendar(fmt::view text, fmt::view format, int day, int month, int year)
	{
		fmt::vector command { "--calendar" };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}
		if (not empty(format))
		{
			command.emplace_back(param("--format", format));
		}
		if (0 < day)
		{
			command.emplace_back(param("--day", fmt::to_string(day)));
		}
		if (0 < month)
		{
			command.emplace_back(param("--month", fmt::to_string(month)));
		}
		if (0 < year)
		{
			command.emplace_back(param("--year", fmt::to_string(year)));
		}

		return dialog(command);
	}

	fmt::page shell::color(fmt::view start, bool palette)
	{
		fmt::vector command { "--color-selection" };

		if (not empty(start))
		{
			command.emplace_back(param("--color", start));
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
	env::shell sh;
	const auto list = sh.list(env::var::pwd());
	assert(not list.empty());
	const auto copy = sh.copy(__FILE__);
	assert(not copy.empty());
	// Copy range starts at 0, file numbering at 1
	assert(copy[__LINE__-1].find("Recursive find me text") != fmt::npos);
}
#endif