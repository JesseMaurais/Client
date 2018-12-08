#ifndef fdbuf_hpp
#define fdbuf_hpp

#include "file.hpp"
#include "iobuf.hpp"

namespace sys::io
{
	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits
	>
	class basic_fdbuf : public basic_iobuf<Char, Traits>
	{
		using base = basic_iobuf<Char, Traits>;

	public:

		using size_type = typename base::size_type;
		using char_type = typename base::char_type;

		basic_fdbuf(int fd = -1)
		{
			set(fd);
		}

		void set(int fd)
		{
			file.set(fd);
		}

		size_type xsputn(char_type const *s, size_type n) override
		{
			size_type const size = n * sizeof (char_type);
			return file.write(static_cast<const void*>(s), size);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			size_type const size = n * sizeof (char_type);
			return file.read(static_cast<void*>(s), size);
		}

	private:

		sys::file::descriptor file;
	};

	using fdbuf = basic_fdbuf<char>;
	using wfdbuf = basic_fdbuf<wchar_t>;
}

#endif // file