#ifndef env_hpp
#define env_hpp

#include "str.hpp"
#include "tmp.hpp"

namespace env
{
	namespace var
	{
		bool got(fmt::string_view);
		fmt::string_view get(fmt::string_view);
		bool set(fmt::string_view);
		bool put(fmt::string_view);
		bool put(fmt::string_view, fmt::string_view);
		fmt::string value(fmt::string_view);
	}

	using size = property<fmt::size_type>;
	using view = property<fmt::string_view>;
	using span = property<fmt::span_view>;
	using pair = property<fmt::pair_view_span>;

	// Use with fmt::dir
	extern view const& os;
	extern span const& paths;
	extern view const& home;
	extern view const& user;
	extern view const& host;
	extern view const& root;
	extern view const& pwd;
	extern view const& lang;
	extern view const& shell;
	extern view const& tmpdir;
	extern view const& rootdir;
	extern view const& session;
	extern view const& domain;
	extern view const& prompt;
}

#endif // file
