#ifndef env_hpp
#define env_hpp

#include "str.hpp"
#include <vector>

namespace sys::env
{
	std::string_view get(std::string_view u);
	bool put(std::string_view u);
	bool set(std::string_view u, std::string_view v);
	std::string eval(std::string s);
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

	extern list const& path;
	extern view const& pwd;
	extern view const& user;
	extern view const& home;
	extern view const& lang;
	extern view const& tmpdir;
	extern view const& shell;
	extern view const& prompt;
	extern view const& term;
	extern view const& editor;
	extern view const& visual;
	extern view const& browser;
	extern view const& pager;
	extern view const& random;
	extern view const& desktop;
}

namespace sys
{
	extern ::env::list const& environment;
}

#endif // file
