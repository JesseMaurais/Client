#ifndef pipebuf_hpp
#define pipebuf_hpp

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
	class basic_pipebuf : public basic_membuf<Char, Traits, Alloc>
	{
		using base = basic_membuf<Char, Traits, Alloc>;

	public:

		using char_type = typename base::char_type;
		using size_type = typename base::size_type;

		basic_pipebuf(int fd[2] = nullptr)
		: file(fd)
		{ }

		void set(int fd[2] = nullptr)
		{
			file.set(fd);
		}

	private:

		sys::file::pipe file;

	protected:

		size_type xsputn(char_type const *s, size_type n) override
		{
			return file[1].write(s, sizeof (char_type) * n);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			return file[0].read(s, sizeof (char_type) * n);
		}
	};

	using pipebuf = basic_pipebuf<char>;
	using wpipebuf = basic_pipebuf<wchar_t>;
}

#endif // file
