#ifndef fd_hpp
#define fd_hpp

#include <string>
#include <iostream>
#include "file.hpp"
#include "fdbuf.hpp"
#include "membuf.hpp"

namespace sys::io
{
	namespace
	{
		template
		<
		 class Char,
		 template <class> class Traits,
		 template <class> class Alloc,
		 template <class, class> class basic_stream,
		 sys::file::openmode default_mode
		>
		class impl_fdstream
		: public basic_stream<Char, Traits<Char>>
		, public basic_membuf<Char, Traits, Alloc>
		, public basic_fdbuf<Char, Traits>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using membuf = basic_membuf<Char, Traits, Alloc>;
			using fdbuf = basic_fdbuf<Char, Traits>;

		public:

			impl_fdstream(int fd = -1)
			: fdbuf(fd)
			, membuf()
			, base(this)
			{ }

			impl_fdstream(std::string const& path, sys::file::openmode mode)
			: impl_fdstream()
			{
				open(path, mode);
			}

			void close() { fd = -1; };
			bool is_open() const { return fd; }
			void open(std::string const& path, sys::file::openmode mode)
			{
				using namespace sys::file;
				if (mode & out and mode & in)
				{
					membuf::setbufsz(BUFSIZ, BUFSIZ);
				}
				else if (mode & out)
				{
					membuf::setbufsz(0, BUFSIZ);
				}
				else if (mode & in)
				{
					membuf::setbufsz(BUFSIZ, 0);
				}
				fd.open(path, mode | default_mode);
			}

		private:

			sys::file::descriptor fd;
		};
	}

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_fdstream
	: public impl_fdstream<Char, Traits, Alloc, std::basic_iostream, sys::file::in|sys::file::out>
	{
		using base = impl_fdstream<Char, Traits, Alloc, std::basic_iostream, sys::file::in|sys::file::out>;
	public:
		using base::base;
	};

	using fdstream = basic_fdstream<char>;
	using wfdstream = basic_fdstream<wchar_t>;

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_ifdstream
	: public impl_fdstream<Char, Traits, Alloc, std::basic_istream, sys::file::in>
	{
		using base = impl_fdstream<Char, Traits, Alloc, std::basic_istream, sys::file::in>;
	public:
		using base::base;
	};

	using ifdstream = basic_ifdstream<char>;
	using wifdstream = basic_ifdstream<wchar_t>;

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_ofdstream
	: public impl_fdstream<Char, Traits, Alloc, std::basic_ostream, sys::file::out>
	{
		using base = impl_fdstream<Char, Traits, Alloc, std::basic_istream, sys::file::in>;
	public:
		using base::base;
	};

	using ofdstream = basic_ofdstream<char>;
	using wofdstream = basic_ofdstream<wchar_t>;
}

#endif // file
