#ifndef env_hpp
#define env_hpp

#include "str.hpp"

namespace env
{
	namespace var
	{
		bool got(fmt::view);
		fmt::view get(fmt::view);
		bool set(fmt::view);
		bool put(fmt::view);
		bool put(fmt::view, fmt::view);
		fmt::string value(fmt::view);
	}

	struct size
	{
		virtual operator fmt::size() const = 0;
	};

	struct view
	{
		virtual operator fmt::view() const = 0;
	};

	struct span
	{
		virtual operator fmt::span<fmt::view>() const = 0;
	};

	extern span const& paths;
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
