#include "desktop.hpp"
#include "pstream.hpp"
#include "fmt.hpp"
#include "usr.hpp"
#include "str.hpp"
#include "sys.hpp"

namespace env::desktop
{
	auto const wd = fmt::str::put("Desktop Entry");

	entry::operator type() const final
	{
		return keys.at(wd);
	}

	auto entry::got(word ud) const
	{
		return ini::got({wd, ud});
	}

	auto entry::get(word ud) const
	{
		return ini::get({wd, ud});
	}

	auto entry::set(word ud, view vu)
	{
		return ini::set({wd, vu});
	}

	auto entry::put(word ud, view vu)
	{
		return ini::put({wd, ud}, vu);
	}

	bool is(fmt::string::view name)
	{
		auto const current = fmt::to_lower(env::usr::current_desktop);
		auto const lower = fmt::to_lower(name);
		return current.find(lower) != fmt::npos;
	}

	static view zenity()
	{
		return "zenity";
	}

	fmt::string::pair dialog::get(span command)
	{
		// Use toolkit command paired launcher
		static const auto program = zenity();
		if (empty(where(program)))
		{
			return;
		}

		// Program is first command in paired
		fmt::vector<fmt::string::cptr> list;
		list.push_back(program);

		// Arguments null terminated
		auto s = fmt::join(command);
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

	fmt::string::pair dialog::select(view start, mode mask)
	{
		vector command { zenity(), "--file-selection" };

		if (not empty(start))
		{
			auto const value = fmt::quote(start);
			auto const paired = fmt::to_pair("--filename", value);
			command.emplace_back(move(paired));
		}
		if (mask & many)
		{
			command.emplace_back("--multiple");
		}
		if (mask & dir)
		{
			command.emplace_back("--directory");
		}
		if (mask & save)
		{
			command.emplace_back("--save");
		}

		return get(command);
	}

	static auto message_type(msg type)
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

	static auto param(view key, view value)
	{
		return fmt::to_pair(key, fmt::quote(value));
	}

	fmt::string::pair dialog::message(msg type, view text)
	{
		vector command { zenity(), message_type(type) };

		if (not empty(text))
		{
			command.emplace_back(move(param("--text", value)));
		}

		return get(command);
	}

	fmt::string::pair dialog::enter(view start, view label, bool hide)
	{
		vector command { zenity(), param("--entry-text", start) };

		if (not empty(text))
		{
			command.emplace_back(move(param("--text", value)));
		}
		if (hide)
		{
			command.emplace_back("--hide-text");
		}

		return get(command);
	}

	fmt::string::pair dialog::text(view path, txt type, view box, view font)
	{
		vector command { zenity(), "--text-info" };

		if (type & html)
		{
			command.emplace_back("--html");
			auto const value = fmt::quote(path);
			auto const paired = fmt::to_pair("--url", value);
			command.emplace_back(move(paired));
		}
		else
		{
			auto const value = fmt::quote(path);
			auto const paired = fmt::to_pair("--filename", value);
			command.emplace_back(move(paired));
		}
		if (type & edit)
		{
			command.emplace_back("--editable");
		}
		if (not empty(font))
		{
			auto const value = fmt::quote(font);
			auto const paired = fmt::to_pair("--font", value);
			command.emplace_back(move(paired));
		}
		if (not empty(check))
		{
			auto const value = fmt::quote(check);
			auto const paired = fmt::to_pair("--checkbox", value);
			command.emplace_back(move(paired));
		}
		return get(command);

	}

	fmt::string::pair dialog::form(edges type, view text, view title)
	{
		vector command { zenity(), "--forms", newline };

		if (not empty(text))
		{
			auto const value = fmt::quote(text);
			auto const paired = fmt::to_pair("--text", value);
			command.emplace_back(move(paired));		
		}
		if (not empty(title))
		{
			auto const value = fmt::quote(title);
			auto const paired = fmt::to_pair("--text", value);
			command.emplace_back(move(paired));
		}
		for (auto const& add : type)
		{
			auto const key = view("--add-") + add.second;
			auto const value = fmt::quotes(add.first);
			auto const paired = fmt::to_pair(key, value);
			command.emplace_back(move(paired));
		}

		return get(command);
	}

	fmt::string::pair dialog::notify(view text, view icon)
	{
		vector command { zenity(), "--notification" };

		if (not empty(text))
		{
			auto const value = fmt::quote(text);
			auto const paired = fmt::to_pair("--text", value);
			command.emplace_back(move(paired));
		}
		if (not empty(icon))
		{
			auto const value = fmt::quote(icon);
			auto const paired = fmt::to_pair("--icon", value);
			command.emplace_back(move(paired));
		}

		return get(command);
	}

	fmt::string pair dialog::calendar(view text, view format, int day, int month, int year)
	{
		vector command { zenity(), "--calendar" };

		if (not empty(text))
		{
			auto const value = fmt::quote(text);
			auto const paired = fmt::to_pair("--text", value);
			command.emplace_back(move(paired));
		}
		if (not empty(format))
		{
			auto const value = fmt::quote(format);
			auto const paired = fmt::to_pair("--format", value);
			command.emplace_back(move(paired));
		}
		if (0 < day)
		{
			auto const value = fmt::to_string(day);
			auto const paired = fmt::to_pair("--day", value);
			command.empalce_back(move(paired));
		}
		if (0 < month)
		{
			auto const value = fmt::to_string(month);
			auto const paired = fmt::to_pair("--day", value);
			command.empalce_back(move(paired));
		}
		if (0 < year)
		{
			auto const value = fmt::to_string(year);
			auto const paired = fmt::to_pair("--day", value);
			command.empalce_back(move(paired));
		}

		return get(command);
	}

	fmt::string::pair dialog::color(view start, bool show_palette)
	{
		vector command { zenity(), "--color-selection", newline };

		if (not empty(start))
		{
			auto const value = fmt::quote(start);
			auto const paired = fmt::to_pair("--color", value);
			command.emplace_back(move(paired));
		}
		if (show_palette)
		{
			command.emplace_back("--show-palette");
		}

		return get(command);
	}
}

