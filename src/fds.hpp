#ifndef fds_hpp
#define fds_hpp

#include <iostream>
#include "file.hpp"
#include "buf.hpp"
#include "str.hpp"

namespace io
{
	namespace impl
	{
		template
		<
		 class Char,
		 template <class> class Traits,
		 template <class> class Alloc,
		 template <class, class> class basic_stream,
		 env::file::mode default_mode
		>
		class basic_fdstream
		: public basic_rwbuf<Char, Traits, Alloc>
		, public basic_stream<Char, Traits<Char>>
		{
			using stream = basic_stream<Char, Traits<Char>>;
			using rwbuf = basic_rwbuf<Char, Traits, Alloc>;
			using string = fmt::basic_string<Char, Traits, Alloc>;
			using string_view = fmt::basic_string_view<Char, Traits>;
			using mode = env::file::mode;
			using size_type = std::size_t;

			env::file::descriptor buf;

		public:

			basic_fdstream(string_view path, mode mask = default_mode, size_type size = env::file::width)
			: rwbuf(buf), stream(this), buf(path, mode(mask | default_mode))
			{
				if (mask & env::file::rw)
				{
					rwbuf::setbufsiz(size, size);
				}
				else 
				if (mask & env::file::wr)
				{
					rwbuf::setbufsiz(0, size);
				}
				else 
				if (mask & env::file::rd)
				{
					rwbuf::setbufsiz(size, 0);
				}
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
	using basic_fdstream = impl::basic_fdstream
	<
	 Char, Traits, Alloc, std::basic_iostream, env::file::rw
	>;

	using fdstream = basic_fdstream<char>;
	using wfdstream = basic_fdstream<wchar_t>;

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	using basic_ifdstream = impl::basic_fdstream
	<
	 Char, Traits, Alloc, std::basic_istream, env::file::rd
	>;

	using ifdstream = basic_ifdstream<char>;
	using wifdstream = basic_ifdstream<wchar_t>;

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	using basic_ofdstream = impl::basic_fdstream
	<
	 Char, Traits, Alloc, std::basic_ostream, env::file::wr
	>;

	using ofdstream = basic_ofdstream<char>;
	using wofdstream = basic_ofdstream<wchar_t>;
}

#endif // file
