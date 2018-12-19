#ifndef env_hpp
#define env_hpp

#include <string_view>
#include <vector>

namespace sys::env
{
	std::string_view get(std::string_view s);
	bool set(std::string_view s, std::string_view v);
};

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
	extern view const& home;
	extern view const& user;
	extern view const& tmpdir;
	extern view const& shell;
	extern view const& term;
	extern view const& pager;
	extern view const& editor;
	extern view const& visual;
	extern view const& random;
};

namespace sys
{
	extern ::env::list const& environment;
};

#endif // file
