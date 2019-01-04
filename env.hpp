#ifndef env_hpp
#define env_hpp

#include "str.hpp"

namespace sys::env
{
	fmt::string_view get(fmt::string_view u);
	bool put(fmt::string_view u);
	bool set(fmt::string_view u, fmt::string_view v);
	fmt::string eval(fmt::string s);
}

namespace env
{
	struct view
	{
		virtual operator fmt::string_view() const = 0;
	};

	struct list
	{
		virtual operator fmt::span_view() const = 0;
	};

	extern list const& path;
	extern view const& user;
	extern view const& home;
	extern view const& pwd;
	extern view const& lang;
	extern view const& shell;
	extern view const& tmpdir;
	extern view const& desktop;
	extern view const& prompt;
}

#endif // file
