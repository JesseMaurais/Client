#ifndef fdbuf_hpp
#define fdbuf_hpp

#include "membuf.hpp"
#include "file.hpp"

namespace sys::io
{
	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits
	 template <class> class Alloc = std::allocator
	>
	class basic_fdbuf : public basic_membuf<Char, Traits, Alloc>
	{
		using base = basic_membuf<Char, Traits, Alloc>;

	public:

		using size_type = typename base::size_type;
		using char_type = typename base::char_type;

		basic_fdbuf(int fd = -1)
		{
			set(fd);
		}

		int set(int fd = -1)
		{
			return file.set(fd);
		}

	private:

		sys::file::descriptor file;

	protected:

		size_type xsputn(char_type const *s, size_type n) override
		{
			return file.write(s, sizeof (char_type) * n);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			return file.read(s, sizeof (char_type) * n);
		}

		void eof(int) override
		{
			file.close();
		}
	};

	using fdbuf = basic_fdbuf<char>;
	using wfdbuf = basic_fdbuf<wchar_t>;
}

#endif // file
