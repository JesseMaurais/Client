#ifndef pbuf_hpp
#define pbuf_hpp

#include "membuf.hpp"
#include "file.hpp"

namespace sys::io
{
	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_pbuf : public basic_membuf<Char, Traits, Alloc>
	{
		using base = basic_membuf<Char, Traits, Alloc>;

	public:

		using char_type = typename base::char_type;
		using size_type = typename base::size_type;

		basic_pbuf(int fd[3] = nullptr)
		: file(fd)
		{ }

		void set(int fd[3] = nullptr)
		{
			file.set(fd);
		}

	private:

		sys::file::process file;

	protected:

		size_type xsputn(char_type const *s, size_type n) override
		{
			return file[0].write(s, sizeof (char_type) * n);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			return file[1].read(s, sizeof (char_type) * n);
		}
	};

	using pbuf = basic_pbuf<char>;
	using wpbuf = basic_pbuf<wchar_t>;
}

#endif // file
