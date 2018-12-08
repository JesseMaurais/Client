#ifndef ipc_hpp
#define ipc_hpp

#include <string>
#include <iostream>
#include "file.hpp"
#include "pipebuf.hpp"

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
		class impl_pstream
		: public basic_stream<Char, Traits<Char>>
		, public basic_pipebuf<Char, Traits>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using pipebuf = basic_pipebuf<Char, Traits>;

		public:

			impl_pstream()
			: pipebuf()
			, base(this)
			{ }

			impl_pstream(sys::file::process::arguments args, sys::file::openmode mode = default_mode)
			: impl_pstream()
			{
				open(args, mode);
			}

			void open(sys::file::process::arguments args, sys::file::openmode mode = default_mode)
			{
				process.open(args, mode | default_mode);
				int fd[2] = { process[0].get(), process[1].get() };
				pipebuf::set(fd);
			}

		private:

			sys::file::process process;
		};
	}

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_pstream
	: public impl_pstream<Char, Traits, Alloc, std::basic_iostream, sys::file::in|sys::file::out>
	{
		using base =  impl_pstream<Char, Traits, Alloc, std::basic_iostream, sys::file::in|sys::file::out>;
	public:
		using base::base;
	};
	
	using pstream = basic_pstream<char>;
	using wpstream = basic_pstream<wchar_t>;

	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_ipstream
	: public impl_pstream<Char, Traits, Alloc, std::basic_istream, sys::file::in>
	{
		using base =  impl_pstream<Char, Traits, Alloc, std::basic_istream, sys::file::in>;
	public:
		using base::base;
	};

	using ipstream = basic_ipstream<char>;
	using wipstream = basic_ipstream<wchar_t>;

	template 
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_opstream
	: public impl_pstream<Char, Traits, Alloc, std::basic_ostream, sys::file::out>
	{
		using base =  impl_pstream<Char, Traits, Alloc, std::basic_ostream, sys::file::out>;
	public:
		using base::base;
	};

	using opstream = basic_opstream<char>;
	using wopstream = basic_opstream<wchar_t>;
}

#endif // file