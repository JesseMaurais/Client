#ifndef env_hpp
#define env_hpp

#include "str.hpp"
#include "tmp.hpp"

namespace env
{
	using fmt::string;

	namespace var
	{
		using string::view;

		bool got(view);
		view get(view);
		bool set(view);
		bool put(view);
		bool put(view, view);
		string value(view);
	}

	template 
	<
		class Type
		template <class> Access = attribute
	>
	struct variable : Access<Type>
	{ 
		using traits = fmt::memory_traits<Type>;
		using traits::cref;
		using traits::ref;
	};

	template
	<
		class Type
	>
	using constant = variable<Type, property>;

	using size = constant<string::size_type>;
	using view = constant<string::view>;
	using span = constant<string::view::span>;
	using pair = constant<std::pair<string::view, string::view::span>>;

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
