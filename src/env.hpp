#ifndef env_hpp
#define env_hpp "Environment Variables"
 
#include "fmt.hpp"
#include "tmp.hpp"

namespace env
{
	using string = fmt::string;

	namespace var
	{
		using view = string::view;

		bool got(view);
		view get(view);
		bool set(view);
		bool put(view);
		bool put(view, view);
		string value(view);
	}

	template 
	<
		class Type,
		template <class> class Traits = fmt::memory_brief,
		template <class> class Access = fwd::variable
	>
	using variable = fwd::compose<Access<Type>, Traits<Type>>;

	template
	<
		class Type,
		template <class> class Traits = fmt::memory_brief
	>
	using constant = variable<const Type, Traits, fwd::constant>;

	using size = constant<string::size_type>;
	using view = constant<string::view>;
	using span = constant<string::view::span>;
	using line = constant<string::view::line>;
	using page = constant<string::view::page>;
	using node = constant<string::view::edges>;

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
