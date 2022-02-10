#ifndef io_hpp
#define io_hpp "Input/Output Streams"

#include <cstring>
#include <streambuf>
#include "file.hpp"
#include "dig.hpp"
#include "fwd.hpp"
#include "ptr.hpp"
#include "tmp.hpp"

namespace fmt
{
	template
	<
		class Char,
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator,
		// details
		class Base = fwd::basic_buf<Char, Traits>
	>
	struct basic_streambuf : Base
	{
		using char_type = typename Base::char_type;
		using traits_type = typename Base::traits_type;
		using int_type = typename Base::int_type;
		using char_ptr = fwd::as_ptr<char_type>;
		using size_type = std::streamsize;

		Base* setbuf(char_ptr s, size_type n, size_type m)
		{
			auto t = s + n;
			auto u = t + m;
			Base::setg(s, t, t);
			Base::setp(t, u);
			return this;
		}

		Base* setbuf(char_ptr s, size_type n) override
		{
			size_type const m = n / 2;
			return setbuf(s, n - m, m);
		}

	protected:

		int_type overflow(int_type c) override
		{
			constexpr int_type eof = traits_type::eof();
			if (Base::pptr() == Base::epptr())
			{
				if (-1 == sync()) c = eof;
			}
			if (traits_type::eq_int_type(eof, c))
			{
				Base::setp(nullptr, nullptr);
			}
			else
			{
				*Base::pptr() = traits_type::to_char_type(c);
			}
			return traits_type::not_eof(c);
		}

		int_type underflow() override
		{
			if (Base::gptr() == Base::egptr())
			{
				auto const max = Base::egptr() - Base::eback();
				auto const n = Base::sgetn(Base::eback(), max);
				if (0 < n)
				{
					auto const diff =  max - n;
					auto const sz = n * sizeof (char_type);
					std::memmove(Base::eback() + diff, Base::eback(), fmt::to_size(sz));
					Base::gbump((int) -n);
				}
				else
				{
					Base::setg(nullptr, nullptr, nullptr);
					return traits_type::eof();
				}
			}
			return traits_type::to_int_type(*Base::gptr());
		}

		int sync() override
		{
			if (Base::pbase() != Base::pptr())
			{
				auto const off = Base::pptr() - Base::pbase();
				auto const n = Base::sputn(Base::pbase(), off);
				if (0 < n)
				{
					auto const diff = off - n;
					auto const sz = diff * sizeof (char_type);
					std::memmove(Base::pbase(), Base::pbase() + n, fmt::to_size(sz));
					Base::pbump((int) -n);
				}
				return n < 0 ? -1 : 0;
			}
			return Base::pptr() != Base::epptr() ? 0 : -1;
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
	struct basic_buf : basic_streambuf<Char, Traits, Alloc>
	{
		using Base = basic_stringbuf<Char, Traits, Alloc>;
		using size_type = typename Base::size_type;
		using char_type = typename Base::char_type;

		basic_buf(env::file::unique_ptr that)
		{
			file = std::move(that);
		}

		basic_buf(env::file::shared_ptr that)
		{
			file = that;
		}

	protected:

		env::file::shared_ptr file;

	private:

		size_type xsputn(char_type const *s, size_type n) override
		{
			const auto m = std::fwrite(s, fmt::to_size(n), sizeof(char), file.get());
			return fmt::to<size_type>(m);
		}

		size_type xsgetn(char_type *s, size_type n) override
		{
			const auto m = std::fread(s, fmt::to_size(n), sizeof(char), file.get());
			return fmt::to<size_type>(m);
		}

		using Base::Base;
	};

	template
	<
		template <class, template <class> class> class Stream,
		class Char,
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator
	>
	struct basic_stream : fwd::no_copy, Stream<Char, Traits>, basic_buf<Char, Traits, Alloc>
	{
		using stream = Stream<Char, Traits>;
		using buf = basic_buf<Char, Traits, Alloc>;

		basic_stream(env::file::unique_ptr && file)
		: stream(this), buf(file)
		{ }
	};

	template
	<
		class Char,
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator
	>
	using basic_istream = basic_stream
	<
		fwd::basic_istream, Char, Traits, Alloc
	>;

	using istream = basic_istream<char>;
	using wistream = basic_istream<wchar_t>;

	template
	<
		class Char,
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator
	>
	using basic_ostream = basic_stream
	<
		fwd::basic_ostream, Char, Traits, Alloc
	>;

	using ostream = basic_ostream<char>;
	using wostream = basic_ostream<wchar_t>;

	template
	<
		class Char,
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator
	>
	using basic_iostream = basic_stream
	<
		fwd::basic_iostream, Char, Traits, Alloc
	>;

	using iostream = basic_iostream<char>;
	using wiostream = basic_iostream<wchar_t>;
}

#endif // file
