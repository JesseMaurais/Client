#ifndef ipc_hpp
#define ipc_hpp

#include <string>
#include <iostream>
#include "pipebuf.hpp"

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
		, public basic_pipebuf<Char, Traits, Alloc>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using pipebuf = basic_pipebuf<Char, Traits, Alloc>;
			using arguments = sys::file::process::arguments;
			using openmode = sys::file::openmode;

		public:

			impl_pstream(std::size_t sz = sys::file::bufsiz)
			: size(sz)
			, pipebuf()
			, base(this)
			{ }

			impl_pstream(arguments args, openmode mode = default_mode)
			: impl_pstream()
			{
				open(args, mode);
			}

			void open(arguments args, openmode mode = default_mode)
			{
				file.open(args, mode | default_mode);
				int fd[2] = { file[1].get(), file[0].get() };
				pipebuf::setbufsiz(size);
				pipebuf::set(fd);
				file.set();
			}

		private:

			sys::file::process file;
			std::size_t const size;
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