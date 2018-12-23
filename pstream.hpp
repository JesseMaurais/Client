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
		 template <class, class> class basic_stream,
		 sys::file::openmode default_mode
		>
		class basic_pstream
		: public basic_stream<Char, Traits<Char>>
		, public basic_pbuf<Char, Traits, Alloc>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using pbuf = basic_pbuf<Char, Traits, Alloc>;
			using arguments = sys::file::arguments;
			using openmode = sys::file::openmode;

		public:

			basic_pstream(std::size_t sz = sys::file::bufsiz)
			: pbuf()
			, base(this)
			{
				pbuf::setbufsiz(sz);
			}

			basic_pstream(arguments args, openmode mode = default_mode)
			: basic_pstream()
			{
				execute(args, mode);
			}

			bool execute(arguments args, openmode mode = default_mode)
			{
				return pbuf::file.execute(args, mode | default_mode);
			}

			void close() { pbuf::file[0].close(); }
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
	 std::basic_iostream,
	 sys::file::in|sys::file::out
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
	 std::basic_istream,
	 sys::file::in
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
	 std::basic_ostream,
	 sys::file::out
	>;

	using opstream = basic_opstream<char>;
	using wopstream = basic_opstream<wchar_t>;
}

#endif // file
