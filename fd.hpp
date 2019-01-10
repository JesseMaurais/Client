#ifndef fd_hpp
#define fd_hpp

#include <string>
#include <iostream>
#include "file.hpp"
#include "fdbuf.hpp"

namespace sys::io
{
	namespace impl
	{
		template
		<
		 class Char,
		 template <class> class Traits,
		 template <class> class Alloc,
		 template <class, class> basic_stream,
		 sys::file::openmode default_mode
		>
		class basic_fdstream
		: public basic_fdbuf<Char, Traits, Alloc>
		, public basic_stream<Char, Traits<Char>>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using fdbuf = basic_fdbuf<Char, Traits, Alloc>;
			using string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;
			using openmode = sys::file::openmode;

		public:

			basic_fdstream(std::size_t sz = sys::file::bufsiz, int fd = -1)
			: size(sz)
			, fdbuf(fd)
			, base(this)
			{ }

			basic_fdstream(string const& path, openmode mode = default_mode)
			: basic_fdstream()
			{
				open(path, mode);
			}

			void open(string const& path, openmode mode = default_mode)
			{
				if (mode & sys::file::out and mode & sys::file::in)
				{
					fdbuf::setbufsiz(size, size);
				}
				else if (mode & sys::file::out)
				{
					fdbuf::setbufsiz(0, size);
				}
				else if (mode & sys::file::in)
				{
					fdbuf::setbufsiz(size, 0);
				}
				file.open(path, mode | default_mode);
			}

			bool close()
			{
				return file.close();
			}

			bool is_open() const
			{
				return !!file;
			}

		private:

			sys::file::descriptor file;
			std::size_t const size;
		};
	}

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	using basic_fdstream = impl::basic_fdstream
	<
	 Char, Traits, Alloc,
	 std::basic_iostream,
	 sys::file::in|sys::file::out
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
	 Char, Traits, Alloc,
	 std::basic_istream,
	 sys::file::in
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
	 Char, Traits, Alloc,
	 std::basic_ostream,
	 sys::file::out
	>;

	using ofdstream = basic_ofdstream<char>;
	using wofdstream = basic_ofdstream<wchar_t>;
}

#endif // file
