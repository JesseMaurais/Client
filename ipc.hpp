#ifndef ipc_hpp
#define ipc_hpp

#include <iostream>
#include "file.hpp"
#include "mem.hpp"

namespace sys::io
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
	class basic_procbuf : public basic_membuf<Char, Traits, Alloc>
	{
		using base = basic_membuf<Char, Traits, Alloc>;

	public:

		using char_type = typename base::char_type;
		using size_type = typename base::size_type;
		using arguments = sys::file::arguments;

		basic_procbuf(int fd[3] = nullptr)
		: file(fd)
		{ }

		void set(int fd[3] = nullptr)
		{
			file.set(fd);
		}

		bool execute(arguments args)
		{
			return file.execute(args);
		}

		void terminate()
		{
			file.terminate();
		}

		void close(int n)
		{
			file.close(n);
		}

	protected:

		sys::file::process file;

		size_type xsputn(char_type const *s, size_type n) override
		{
			return file[0].write(s, sizeof (char_type) * n);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			return file[1].read(s, sizeof (char_type) * n);
		}
	};

	// Common alias types

	using procbuf = basic_procbuf<char>;
	using wprocbuf = basic_procbuf<wchar_t>;

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
		 template <class, class> class basic_stream
		>
		class basic_pstream
		: public basic_stream<Char, Traits<Char>>
		, public basic_procbuf<Char, Traits, Alloc>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using procbuf = basic_procbuf<Char, Traits, Alloc>;
			using arguments = typename procbuf::arguments;

		public:

			basic_pstream(std::size_t sz = sys::file::bufsiz)
			: procbuf()
			, base(this)
			{
				this->setbufsiz(sz);
			}

			basic_pstream(arguments args)
			: basic_pstream()
			{
				this->execute(args);
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
