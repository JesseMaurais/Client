#ifndef pstream_hpp
#define pstream_hpp

#include <string>
#include <iostream>
#include "pbuf.hpp"
#include "file.hpp"

namespace sys::io
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
		: public basic_stream<Char, Traits<Char>>
		, public basic_pbuf<Char, Traits, Alloc>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using arguments = sys::file::arguments;

		public:

			basic_pstream(std::size_t sz = sys::file::bufsiz)
			: pbuf()
			, base(this)
			{
				this->setbufsiz(sz);
			}

			basic_pstream(arguments args)
			: basic_pstream()
			{
				execute(args);
			}

			bool execute(arguments args)
			{
				return this->file.execute(args);
			}
		};
	}

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
