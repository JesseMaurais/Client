#ifndef fds_hpp
#define fds_hpp

#include <iostream>
#include "file.hpp"
#include "buf.hpp"
#include "str.hpp"

namespace io
{
	//
	// Abstract buffer class
	//

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_fdbuf 
	: public basic_rwbuf<sys::file::descriptor, Char, Traits, Alloc>
	{
		using base = basic_rwbuf<sys::file::descriptor, Char, Traits, Alloc>;

	public:

		using size_type = typename base::size_type;
		using char_type = typename base::char_type;
		using base::base;
	};

	// Common alias types

	using fdbuf = basic_fdbuf<char>;
	using wfdbuf = basic_fdbuf<wchar_t>;

	//
	// Abstract stream class
	//

	namespace impl
	{
		template
		<
		 class Char,
		 template <class> class Traits,
		 template <class> class Alloc,
		 template <class, class> class basic_stream,
		 sys::file::mode default_mode
		>
		class basic_fdstream
		: public basic_fdbuf<Char, Traits, Alloc>
		, public basic_stream<Char, Traits<Char>>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using fdbuf = basic_fdbuf<Char, Traits, Alloc>;
			using string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;
			using string_view = fmt::basic_string_view<Char, Traits<Char>>;
			using mode = sys::file::mode;
			using size_type = std::size_t;

		public:

			basic_fdstream(string_view path, mode mask = default_mode, size_type size = sys::file::bufsiz())
			: fdbuf(path, mode(mask | default_mode))
			, base(this)
			{
				if (mask & sys::file::rw)
				{
					fdbuf::setbufsiz(size, size);
				}
				else 
				if (mask & sys::file::wr)
				{
					fdbuf::setbufsiz(0, size);
				}
				else 
				if (mask & sys::file::rd)
				{
					fdbuf::setbufsiz(size, 0);
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
	 Char, Traits, Alloc, std::basic_iostream, sys::file::rw
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
	 Char, Traits, Alloc, std::basic_istream, sys::file::rd
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
	 Char, Traits, Alloc, std::basic_ostream, sys::file::wr
	>;

	using ofdstream = basic_ofdstream<char>;
	using wofdstream = basic_ofdstream<wchar_t>;
}

#endif // file
