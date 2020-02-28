#ifndef ips_hpp
#define ips_hpp // Inter-Process Stream

#include <initializer_list>
#include <iostream>
#include <vector>
#include "mode.hpp"
#include "proc.hpp"
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

			basic_pstream(env::file::command line, size_type sz = env::file::width) 
			: buf(f), stream(this), f(line)
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
