#ifndef env_hpp
#define env_hpp

#include <string_view>
#include <vector>

namespace sys::env
{
	std::string_view get(std::string_view s);
	bool set(std::string_view k, std::string_view v);
}

namespace env
{
	struct view
	{
		virtual operator std::string_view() const = 0;
	};

	struct list
	{
		virtual operator std::vector<std::string_view>() const = 0;
	};

	list const& path;
	view const& pwd;
	view const& user;
	view const& home;
	view const& tmpdir;
	view const& shell;
	view const& prompt;
	view const& term;
	view const& editor;
	view const& visual;
	view const& browser;
	view const& pager;
	view const& random;
	view const& desktop;
}

namespace sys
{
	::env::list const& environment;
}

#endif // file