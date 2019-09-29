#ifndef ips_hpp
#define ips_hpp

#include <initializer_list>
#include <iostream>
#include <vector>
#include "ipc.hpp"
#include "buf.hpp"

namespace io
{
	namespace impl
	{
		template
		<
		 class Char,
		 template <class> class Traits,
		 template <class> class Alloc,
		 template <class, class> class basic_stream
		>
		class basic_pstream
		: public basic_rwbuf<Char, Traits, Alloc>
		, public basic_stream<Char, Traits<Char>>
		{
			using stream = basic_stream<Char, Traits<Char>>;
			using rwbuf = basic_rwbuf<Char, Traits, Alloc>;

			env::file::process buf;

		public:

			basic_pstream(env::file::command line) 
			: rwbuf(buf), stream(this), buf(line)
			{
				auto const sz = env::file::width();
				rwbuf::setbufsiz(sz);
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
