#ifndef ipc_hpp
#define ipc_hpp

#include <string>
#include <iostream>
#include <initializer_list>
#include "file.hpp"
#include "membuf.hpp"
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
		: basic_stream<Char, Traits<Char>>
		, basic_membuf<Char, Traits, Alloc>
		, basic_pipebuf<Char, Traits>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using membuf = basic_membuf<Char, Traits, Alloc>;
			using pipebuf = basic_fdbuf<Char, Traits>;

		public:

			impl_pstream()
			: pipebuf()
			, membuf()
			, base(this)
			{ }

			impl_pstream(std::initializer_list<char*> args, sys::file::openmode mode = default_mode)
			: impl_pstream()
			{
				open(args, mode);
			}

			bool is_open() const { return pipe; }
			void open(std::initializer_list<char*> args, sys::file::openmode mode = default_mode)
			{
				pipe.open(args, mode | default_mode);
			}

		private:

			sys::file::pipe pipe;
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
