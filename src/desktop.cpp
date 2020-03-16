#include "desktop.hpp"
#include "fmt.hpp"
#include "usr.hpp"

namespace env::desktop
{
	bool is(fmt::string_view name)
	{
		auto const current = fmt::to_lower(env::usr::current_desktop);
		auto const lower = fmt::to_lower(name);
		return current.find(lower) != fmt::npos;
	}

	dialog::dialog(fmt::span_view args)
	{
		constexpr auto program = "zenity";

		if (empty(where(program)))
		{
			return;
		}

		fmt::vector<fmt::string::const_pointer> list;

		auto s = fmt::join(args);
		for (auto u : fmt::split(s))
		{
			list.push_back(data(u));
		}
		list.push_back(nullptr);

		if (fail(start(list.size() - 1, list.data())))
		{
			sys::err(here, program, s);
		}
	}
}

