#ifndef pipebuf_hpp
#define pipebuf_hpp

#include "membuf.hpp"
#include "fdbuf.hpp"

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
		using fdbuf = basic_fdbuf<Char, Traits, Alloc>;

	public:

		using char_type = typename base::char_type;
		using size_type = typename base::size_type;

		basic_pipebuf(int fd[2] = nullptr)
		{
			set(fd);
		}

		void set(int fd[2] = nullptr)
		{
			for (int i : { 0, 1 })
			{
				buf[i].set(fd ? fd[i] : -1);
			}
		}

	private:

		fdbuf buf[2];

	protected:

		size_type xsputn(char_type const *s, size_type n) override
		{
			return buf[1].xsputn(s, n);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			return buf[0].xsgetn(s, n);
		}
	};

	using pipebuf = basic_pipebuf<char>;
	using wpipebuf = basic_pipebuf<wchar_t>;
}

#endif // file