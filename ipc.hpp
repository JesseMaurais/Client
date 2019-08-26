#ifndef ipc_hpp
#define ipc_hpp

#include <initializer_list>
#include <iostream>
#include <vector>
#include "file.hpp"
#include "fds.hpp"

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
	class basic_processbuf 
	: public std::basic_streambuf<Char, Traits<Char>>
	{
		using base = std::basic_streambuf<Char, Traits<Char>>;
		using fdbuf = basic_fdbuf<Char, Traits, Alloc>;

	public:

		using char_type = typename base::char_type;
		using size_type = typename std::streamsize;
		using arguments = std::initializer_list<char const*>;

		fdbuf cin, cout, cerr;

		basic_processbuf(std::size_t sz = sys::file::bufsiz)
		{
			cin.setbufsiz(0, sz);
			cout.setbufsiz(sz, 0);
			cerr.setbufsiz(sz, 0);
		}

	protected:

		size_type xsputn(char_type const *s, size_type n) override
		{
			return cin.sputn(s, n);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			return cout.sgetn(s, n);
		}
	};

	// Common alias types

	using processbuf = basic_processbuf<char>;
	using wprocessbuf = basic_processbuf<wchar_t>;

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
		: public basic_processbuf<Char, Traits, Alloc>
		, public basic_stream<Char, Traits<Char>>
		{
			using base = basic_stream<Char, Traits<Char>>;
			using processbuf = basic_processbuf<Char, Traits, Alloc>;
			using arguments = typename processbuf::arguments;

		public:

			basic_pstream(arguments args)
			: processbuf()
			, base(this)
			{
				std::vector argv(args);
				argv.push_back(nullptr);
				file.run(data(argv));
				if (not fail(file))
				{
					this->cin.set(file[0].set());
					this->cout.set(file[1].set());
					this->cerr.set(file[2].set());
				}
			}

			void kill()
			{
				file.kill();
			}
	
			int wait()
			{
				file.wait();
			}

		private:

			sys::file::process file;
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
