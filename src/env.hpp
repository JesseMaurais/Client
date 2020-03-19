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

	template 
	<
		template <class> class Access
		class Type
	>
	struct variable : Access<Type>
	{ 
		using traits = fmt::memory_traits<Type>;
		using type = traits::type;
		using ref = traits::ref;
	};

	using size = variable<property, fmt::string::size_type>;
	using view = variable<property, fmt::string::view>;
	using span = variable<property, fmt::string::view::span>;
	using pair = variable<property, fmt::pair_view_span>;

	extern view::ref os;
	extern span::ref paths;
	extern view::ref home;
	extern view::ref user;
	extern view::ref host;
	extern view::ref root;
	extern view::ref pwd;
	extern view::ref lang;
	extern view::ref shell;
	extern view::ref tmpdir;
	extern view::ref rootdir;
	extern view::ref session;
	extern view::ref domain;
	extern view::ref prompt;
}

#endif // file
