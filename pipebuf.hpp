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

		basic_pipebuf() = default;
		basic_pipebuf(int fd[2])
		{
			setfd(fd);
		}

		void setfd(int fd[2])
		{
			this->fd[0].setfd(fd[0]);
			this->fd[1].setfd(fd[1]);
		}

	private:

		fdbuf fd[2];

	protected:

		size_type xsputn(char_type const *s, size_type n) override
		{
			return fd[1].xsputn(s, n);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			return fd[0].xsgetn(s, n);
		}
	};

	using pipebuf = basic_pipebuf<char>;
	using wpipebuf = basic_pipebuf<wchar_t>;
}

#endif // file