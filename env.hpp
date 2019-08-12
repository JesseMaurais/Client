#ifndef env_hpp
#define env_hpp

#include "str.hpp"
#include "ptr.hpp"

namespace env
{
	struct value : unique
	{
		virtual operator long() const = 0;
	};

	struct view : unique
	{
		virtual operator fmt::string_view() const = 0;
	};

	struct list : unique
	{
		virtual operator fmt::string_view_span() const = 0;
	};
}

namespace sys::env
{
	fmt::string_view get(fmt::string_view u);
	bool put(fmt::string_view u);
	bool set(fmt::string_view u, fmt::string_view v);
	fmt::string value(fmt::string_view);
}

namespace env
{
	extern list const& paths;
	extern view const& home;
	extern view const& user;
	extern view const& host;
	extern view const& domain;
	extern view const& root;
	extern view const& pwd;
	extern view const& lang;
	extern view const& shell;
	extern view const& tmpdir;
	extern view const& rootdir;
	extern view const& desktop;
	extern view const& prompt;
}

#endif // file
