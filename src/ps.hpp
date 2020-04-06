#ifndef ps_hpp
#define ps_hpp "Process Stream"

#include <initializer_list>
#include <iostream>
#include <vector>
#include "process.hpp"
#include "mode.hpp"
#include "buf.hpp"

namespace fmt
{
	namespace impl
	{
		template
		<
		 class Char,
		 template <class> class Traits,
		 template <class> class Alloc,
		 template <class, class> class Stream
		>
		class basic_pstream
		: public basic_buf<Char, Traits, Alloc>
		, public Stream<Char, Traits<Char>>
		{
			using stream = Stream<Char, Traits<Char>>;
			using buf = basic_buf<Char, Traits, Alloc>;

			env::file::process f;

		public:

			basic_pstream(fmt::list_view args, size_type sz = env::file::width) 
			: buf(f), stream(this), f(args)
			{
				buf::setbufsiz(sz, sz);
			}
		};
	}

	// Common alias types

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	using basic_pstream = impl::basic_pstream
	<
	 Char, Traits, Alloc,
	 std::basic_iostream
	>;
	
	using pstream = basic_pstream<char>;
	using wpstream = basic_pstream<wchar_t>;

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	using basic_ipstream = impl::basic_pstream
	<
	 Char, Traits, Alloc,
	 std::basic_istream
	>;

	using ipstream = basic_ipstream<char>;
	using wipstream = basic_ipstream<wchar_t>;

	template 
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	using basic_opstream = impl::basic_pstream
	<
	 Char, Traits, Alloc,
	 std::basic_ostream
	>;

	using opstream = basic_opstream<char>;
	using wopstream = basic_opstream<wchar_t>;
}

#endif // file
