#ifndef pipebuf_hpp
#define pipebuf_hpp

#include "iobuf.hpp"
#include "fdbuf.hpp"

namespace sys::io
{
	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits
	>
	class basic_pipebuf : public basic_iobuf<Char, Traits>
	{
		using base = basic_iobuf<Char, Traits>;
		using fdbuf = basic_fdbuf<Char, Traits>;

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

		void eof()
		{
			fbuf[0].eof();
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