#ifndef env_hpp
#define env_hpp

#include <string_view>
#include <vector>

namespace sys::env
{
	std::string_view get(std::string_view u);
	bool put(std::string_view u);
	bool set(std::string_view u, std::string_view v);
	std::string format(std::string s);
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
	view const& lang;
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