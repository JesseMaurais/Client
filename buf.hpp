#ifndef buf_hpp
#define buf_hpp

#include <cstring>
#include <streambuf>
#include "int.hpp"
#include "str.hpp"
#include "ops.hpp"

namespace io
{
	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_streambuf : public std::basic_streambuf<Char, Traits<Char>>
	{
		using base = std::basic_streambuf<Char, Traits<Char>>;
		using ios = std::basic_ios<Char, Traits<Char>>;
		using string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;
		using string_view = fmt::basic_string_view<Char, Traits<Char>>;

	public:

		using char_type = typename base::char_type;
		using traits_type = typename base::traits_type;
		using size_type = std::streamsize;
		using int_type = typename base::int_type;

		base *setbuf(char_type *s, size_type n) override
		{
			size_type const m = n / 2;
			return setbuf(s, n - m, m);
		}

		base *setbuf(char_type *s, size_type n, size_type m)
		{
			auto t = s + n;
			auto u = t + m;
			base::setg(s, t, t);
			base::setp(t, u);
			return this;
		}

		string_view pview() const
		{
			auto const sz = base::pptr() - base::pbase();
			return string_view(base::pbase(), fmt::to_size(sz));
		}

		string_view gview() const
		{
			auto const sz = base::egptr() - base::gptr();
			return string_view(base::gptr(), sz);
		}

	protected:

		int_type overflow(int_type c) override
		{
			constexpr int_type eof = traits_type::eof();
			if (base::pptr() == base::epptr())
			{
				if (-1 == sync()) c = eof;
			}
			if (traits_type::eq_int_type(eof, c))
			{
				base::setp(nullptr, nullptr);
			}
			else
			{
				*base::pptr() = traits_type::to_char_type(c);
			}
			return traits_type::not_eof(c);
		}

		int_type underflow() override
		{
			if (base::gptr() == base::egptr())
			{
				auto const max = base::egptr() - base::eback();
				auto const n = base::sgetn(base::eback(), max);
				if (0 < n)
				{
					auto const diff =  max - n;
					auto const sz = n * sizeof (char_type);
					std::memmove(base::eback() + diff, base::eback(), fmt::to_size(sz));
					base::gbump((int) -n);
				}
				else
				{
					base::setg(nullptr, nullptr, nullptr);
					return traits_type::eof();
				}
			}
			return traits_type::to_int_type(*base::gptr());
		}

		int sync() override
		{
			if (base::pbase() != base::pptr())
			{
				auto const off = base::pptr() - base::pbase();
				auto const n = base::sputn(base::pbase(), off);
				if (0 < n)
				{
					auto const diff = off - n;
					auto const sz = diff * sizeof (char_type);
					std::memmove(base::pbase(), base::pbase() + n, fmt::to_size(sz));
					base::pbump((int) -n);
				}
				return n < 0 ? -1 : 0;
			}
			return base::pptr() != base::epptr() ? 0 : -1;
		}
	};

	using streambuf = basic_streambuf<char>;
	using wstreambuf = basic_streambuf<wchar_t>;


	template
	<
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_stringbuf : public basic_streambuf<Char, Traits, Alloc>
	{
		using base = basic_streambuf<Char, Traits>;
		using string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;

	public:

		using char_type = typename base::char_type;
		using size_type = typename base::size_type;

		basic_stringbuf() = default;
		basic_stringbuf(size_type n)
		{
			setbufsiz(n);
		}

		auto setbufsiz(size_type n)
		{
			buf.resize(fmt::to_size(n));
			return base::setbuf(buf.data(), n);
		}

		auto setbufsiz(size_type n, size_type m)
		{
			buf.resize(fmt::to_size(n + m));
			return base::setbuf(buf.data(), n, m);
		}

	private:

		string buf;
	};

	using stringbuf = basic_stringbuf<char>;
	using wstringbuf = basic_stringbuf<wchar_t>;


	template
	<
	 class RW,
	 class Char,
	 template <class> class Traits = std::char_traits,
	 template <class> class Alloc = std::allocator
	>
	class basic_rwbuf : public basic_stringbuf<Char, Traits, Alloc>
	{
		using base = basic_stringbuf<Char, Traits, Alloc>;

	public:

		using size_type = typename base::size_type;
		using char_type = typename base::char_type;

	protected:

		RW ops;

	private:

		size_type xsputn(char_type const *s, size_type n) override
		{
			auto const sz = fmt::to_size(sizeof (char_type) * n);
			return ops.write(s, sz);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			auto const sz = fmt::to_size(sizeof (char_type) * n);
			return ops.read(s, sz);
		}
	};
}

#endif // file
