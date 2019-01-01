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

	extern list const& PATH;
	extern view const& USER;
	extern view const& HOME;
	extern view const& PWD;
	extern view const& LANG;
	extern view const& SHELL;
	extern view const& TMPDIR;
	extern view const& DESKTOP;
	extern view const& PROMPT;
}

#endif // file
