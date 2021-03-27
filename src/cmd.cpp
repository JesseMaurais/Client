// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "cmd.hpp"
#include "dlg.hpp"
#include "dev.hpp"
#include "usr.hpp"
#include "arg.hpp"
#include "str.hpp"
#include "type.hpp"
#include "dir.hpp"
#include "ps.hpp"
#include "err.hpp"
#include "algo.hpp"
#include <exception>

namespace env
{
	// cmd.hpp

	struct shell& cmd()
	{
		return dlg();
	}

	shell::line shell::get(in put, char end, int count)
	{
		// Result in span at cache end
		const auto first = cache.size();
		auto second = first;
		try // process can crash
		{
			while (--count and std::getline(put, last, end))
			{
				cache.emplace_back(std::move(last));
			}
			// One past the end
			second = cache.size();
		}
		// Put exception message into line
		catch (std::exception const& error)
		{
			last = error.what();
		}
		return { first, second, &cache };
	}

	shell::line shell::run(span arguments)
	{
		fmt::ipstream sub(arguments);
		auto const line = get(sub);
		status = sub.wait();
		return line;
	}

	shell::line shell::run(init arguments)
	{
		view::vector s(arguments);
		return run(s);
	}

	shell::line shell::list(view name)
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

	shell::line shell::copy(view path)
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

	shell::line shell::find(view pattern, view directory)
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

	shell::line shell::which(view name)
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

	shell::line shell::open(view path)
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
				if (empty(session) or desktop::current(session))
				{
					if (auto path = which(program); not path.empty())
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

	// dev.hpp

	struct dump& dev()
	{
		thread_local dump local;
		return local;
	}

	shell::line dump::sym(view path)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dumpbin", "-nologo", "-imports", path };
		#else
			{ "objdump", "-t", path };
		#endif
		return get(sub);
	}

	shell::line dump::dyn(view path)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dumpbin", "-nologo", "-exports", path };
		#else
			{ "objdump", "-T", path };
		#endif
		return get(sub);
	}

	// dlg.hpp

	desktop& dlg()
	{
		thread_local desktop local;
		return local;
	}

	bool desktop::current(fmt::string::view name)
	{
		auto const lower = fmt::to_lower(name);
		auto const current = fmt::to_lower(env::usr::current_desktop());
		return current.find(lower) != fmt::npos;
	}

	shell::line desktop::with(string::span command)
	{
		static opt::name const group = fmt::str::put("Desktop Entry");
		static opt::name const key = fmt::str::put("DIALOG");
		static opt::pair const entry { group, key };
		static view const zenity = "zenity";

		// Look for the Zenity desktop utility program
		auto const program = env::opt::get(entry, zenity);
		if (auto const path = which(program); path.empty())
		{
			return path;
		}

		// Program is first command in paired
		fwd::vector<char const*> list;
		list.push_back(data(program));

		// Arguments null terminated
		constexpr auto del = " ";
		if (auto s = fmt::join(command, del); not empty(s))
		{
			for (auto u : fmt::split(s, del))
			{
				list.push_back(data(u));
			}
		}

		// Command list null terminated
		auto const argc = list.size();
		list.push_back(nullptr);
		auto const argv = list.data();

		// Run process with command
		fmt::ipstream sub 
			{ argc, argv };
		return get(sub);
	}

	static auto param(fmt::string::view key, fmt::string::view value)
	{
		return fmt::join({key, value}, "=");
	}

	shell::line desktop::select(view path, mode mask)
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

		return with(command);
	}

	static auto message_type(desktop::msg type)
	{
		switch (type)
		{
		default:
		case desktop::msg::error:
			return "--error";
		case desktop::msg::info:
			return "--info";
		case desktop::msg::query:
			return "--question";
		case desktop::msg::warn:
			return "--warn";
		}
	};

	shell::line desktop::message(view text, msg type)
	{
		vector command { message_type(type) };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}

		return with(command);
	}

	shell::line desktop::enter(view start, view label, bool hide)
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

		return with(command);
	}

	shell::line desktop::text(view path, view check, view font, txt type)
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
		return with(command);

	}

	shell::line desktop::form(controls add, view text, view title)
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

		return with(command);
	}

	shell::line desktop::notify(view text, view icon)
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

		return with(command);
	}

	shell::line desktop::calendar(view text, view format, int day, int month, int year)
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

		return with(command);
	}

	shell::line desktop::color(view start, bool palette)
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

		return with(command);
	}
}

#ifdef test_unit
test_unit(shell)
{
	struct env::shell sh;
	auto const list = sh.list(env::pwd());
	assert(not empty(list));
	auto const copy = sh.copy(__FILE__);
	assert(not empty(copy));
	// Copy range starts at 0, file numbering at 1
	assert(copy[__LINE__-1].find("Recursive find me text") != fmt::npos);
}
#endif

