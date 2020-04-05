#include "desktop.hpp"
#include "fmt.hpp"
#include "usr.hpp"

namespace env::desktop
{
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

	dialog::dialog(fmt::string::view::span args)
	{
		// Use toolkit command line launcher
		static const auto program = zenity();
		if (empty(where(program)))
		{
			return;
		}

		// Program is first command in line
		fmt::vector<fmt::string::cptr> list;
		list.push_back(program);

		// Arguments null terminated
		auto s = fmt::join(args);
		for (auto u : fmt::split(s))
		{
			list.push_back(u.data());
		}

		// Command line null terminated
		auto const argc = list.size();
		list.push_back(nullptr);
		auto const argv = list.data();

		// Start process with command
		if (fail(start(argc, argv)))
		{
			sys::err(here, program, s);
		}
	}
}

