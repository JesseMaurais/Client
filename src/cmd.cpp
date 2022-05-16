// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "cmd.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "tag.hpp"
#include "io.hpp"

namespace env
{
	shell::page shell::get(in feed, char end, int count)
	{
		// Result in span at cache end
		const auto first = cache.size();
		auto second = first;
		try // process can crash
		{
			std::string line;
			while (count-- and std::getline(feed, line, end))
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
		page result(first, second, &cache);
		return result;
	}

	shell::page shell::run(span args)
	{
		string line;
		for (view arg : args)
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

	shell::page shell::run(init args)
	{
		vector vec { args };
		return run(vec);
	}

	shell::page shell::echo(view line)
	{
		return run({ "echo", line });
	}

	shell::page shell::list(view name)
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

	shell::page shell::copy(view path)
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

	shell::page shell::find(view pattern, view directory)
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

	shell::page shell::which(view name)
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

	shell::page shell::start(view path)
	{
		#ifdef _WIN32
		{
			return run({ "start", "/d", path });
		}
		#else
		{
			string::view::pair const test [] =
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

	shell::page shell::imports(view path)
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

	shell::page shell::exports(view path)
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

	bool shell::desktop(fmt::string::view name)
	{
		auto const lower = fmt::to_lower(name);
		auto const current = fmt::to_lower(env::usr::current_desktop());
		return current.find(lower) != fmt::npos;
	}

	static fmt::string::view::vector envpick()
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

	shell::page shell::dialog(span args)
	{
		// Look for any desktop utility program
		view program;
		static auto const session = envpick();
		for (auto test : session)
		{
			if (auto path = which(test); not path.empty())
			{
				program = path;
				break;
			}
		}
		program = optpick(program);
		// Append the command line
		vector command;
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

	shell::page shell::select(view path, mode mask)
	{
		vector command { "--file-selection" };

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

	shell::page shell::message(view text, msg type)
	{
		vector command { message_type(type) };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}

		return dialog(command);
	}

	shell::page shell::enter(view start, view label, bool hide)
	{
		vector command { param("--entry-text", start) };

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

	shell::page shell::text(view path, view check, view font, txt type)
	{
		vector command { "--text-info" };

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

	shell::page shell::form(controls add, view text, view title)
	{
		vector command { "--forms" };

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

	shell::page shell::notify(view text, view icon)
	{
		vector command { "--notification" };

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

	shell::page shell::calendar(view text, view format, int day, int month, int year)
	{
		vector command { "--calendar" };

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

	shell::page shell::color(view start, bool palette)
	{
		vector command { "--color-selection" };

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
	auto const list = env::shell().list(env::var::pwd());
	assert(not empty(list));
	auto const copy = env::shell().copy(__FILE__);
	assert(not empty(copy));
	// Copy range starts at 0, file numbering at 1
	assert(copy[__LINE__-1].find("Recursive find me text") != fmt::npos);
}
#endif