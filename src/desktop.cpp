// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "desktop.hpp"
#include "type.hpp"
#include "usr.hpp"
#include "arg.hpp"
#include "str.hpp"
#include "algo.hpp"
#include "sys.hpp"
#include "ps.hpp"

namespace env
{
	desktop& dialog()
	{
		thread_local struct desktop dlg;
		return dlg;
	}

	bool desktop::current(fmt::string::view name)
	{
		auto const lower = fmt::to_lower(name);
		auto const current = fmt::to_lower(env::usr::current_desktop());
		return current.find(lower) != fmt::npos;
	}

	shell::line desktop::with(string::span command)
	{
		static opt::word const group = fmt::str::put("Desktop Entry");
		static opt::word const key = fmt::str::put("DIALOG");
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

