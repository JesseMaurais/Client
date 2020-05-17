#include "desktop.hpp"
#include "type.hpp"
#include "usr.hpp"
#include "arg.hpp"
#include "str.hpp"
#include "algo.hpp"
#include "sys.hpp"
#include "ps.hpp"

static auto const wd = fmt::str::put("Desktop Entry");

namespace env::desktop
{
	bool got(view ud)
	{
		return opt::got({wd, fmt::str::set(ud)});
	}

	view get(view ud)
	{
		return opt::get({wd, fmt::str::set(ud)});
	}

	bool set(view ud, view vu)
	{
		return opt::set({wd, fmt::str::set(ud)}, vu);
	}

	bool put(view ud, view vu)
	{
		return opt::put({wd, fmt::str::set(ud)}, vu);
	}

	bool is(fmt::string::view name)
	{
		auto const lower = fmt::to_lower(name);
		auto const current = fmt::to_lower(env::usr::current_desktop);
		return current.find(lower) != fmt::npos;
	}

	shell::line dialog::get(span command)
	{
		// Look for the Zenity dialog utility program
		constexpr auto program = env::opt::get("DIALOG", "zenity");
		if (auto const path = which(program); empty(path))
		{
			return path;
		}

		// Program is first command in paired
		fwd::vector<char const*> list;
		list.push_back(program);

		// Arguments null terminated
		auto s = fwd::join(command);
		for (auto u : fmt::split(s))
		{
			list.push_back(u.data());
		}

		// Command paired null terminated
		auto const argc = list.size();
		list.push_back(nullptr);
		auto const argv = list.data();

		// Start process with command
		fmt::ipstream sub { argc, argv };
		return get(sub);
	}

	static auto param(view key, view value)
	{
		return fmt::to_pair(key, value);
	}

	shell::line dialog::select(view path, mode mask)
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

		return get(command);
	}

	static auto message_type(dialog::msg type)
	{
		switch (type)
		{
		default:
		case dialog::msg::error:
			return "--error";
		case dialog::msg::info:
			return "--info";
		case dialog::msg::query:
			return "--question";
		case dialog::msg::warn:
			return "--warn";
		}
	};

	shell::line dialog::message(view text, msg type)
	{
		vector command { message_type(type) };

		if (not empty(text))
		{
			command.emplace_back(param("--text", value));
		}

		return get(command);
	}

	shell::line dialog::enter(view start, view label, bool hide)
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

		return get(command);
	}

	shell::line dialog::text(view path, view box, view font, txt type)
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
		return get(command);

	}

	shell::line dialog::form(edges type, view text, view title)
	{
		vector command { "--forms", newline };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));		
		}
		if (not empty(title))
		{
			command.emplace_back(param("--text", title));
		}
		for (auto const& add : type)
		{
			auto const key = view("--add-") + add.second;
			command.emplace_back(param(key, add.first));
		}

		return get(command);
	}

	shell::line dialog::notify(view text, view icon)
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

		return get(command);
	}

	shell::line dialog::calendar(view text, view format, int day, int month, int year)
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
			command.empalce_back(param("--day", day));
		}
		if (0 < month)
		{
			command.empalce_back(param("--month", month));
		}
		if (0 < year)
		{
			command.empalce_back(param("--year", year));
		}

		return get(command);
	}

	shell::span dialog::color(view start, bool palette)
	{
		vector command { "--color-selection", newline };

		if (not empty(start))
		{
			command.emplace_back(param("--color", start));
		}
		if (palette)
		{
			command.emplace_back("--show-palette");
		}

		return get(command);
	}
}

