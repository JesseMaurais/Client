#ifndef fmt_hpp
#define fmt_hpp "Standard Format"

#include "fwd.hpp"
#include "algo.hpp"
#include "tmp.hpp"

namespace fmt
{
	template <class Type> struct memory
	{
		using ptr  = typename std::add_pointer<Type>::type;
		using cptr = typename std::add_pointer<typename std::add_const<Type>::type>::type;
		using ref  = typename std::add_lvalue_reference<Type>::type;
		using cref = typename std::add_lvalue_reference<typename std::add_const<Type>::type>::type;

		using next  = fwd::function<cref>;
		using put   = fwd::function<ref>;
		using read  = fwd::accessor<cref>;
		using write = fwd::accessor<ref>;
		using order = fwd::relation<cref>;
		using swap  = fwd::relation<ref>;
		using check = fwd::predicate<cref>;
		using copy  = fwd::predicate<ref>;
	};

	template
	<
		class Char, template <class> class Traits = fwd::character
	>
	struct stream
	{
		using ctype = memory<std::ctype<Char>>;
		using ios   = memory<fwd::basic_ios<Char, Traits>>;
		using in    = memory<fwd::basic_istream<Char, Traits>>;
		using out   = memory<fwd::basic_ostream<Char, Traits>>;
		using io    = memory<fwd::basic_iostream<Char, Traits>>;
		using buf   = memory<fwd::basic_buf<Char, Traits>>;
		using file  = memory<fwd::basic_file<Char, Traits>>;
		using str   = memory<fwd::basic_stringstream<Char, Traits>>;

		using scan   = typename in::put;
		using print  = typename out::put;
		using format = typename ctype::read;

		template <class Iterator> static
		typename out::ref put(typename out::ref buf, Iterator begin, Iterator end, Char* del)
		{
			auto it = std::ostream_iterator(buf, del);
			std::copy(begin, end, it);
			return buf;
		}
	};

	template
	<
		class Type,
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order
	>
	struct layout
	{
		using type   = Type;
		using pair   = fwd::pair<Type>;
		using init   = fwd::init<Type>;
		using set    = fwd::set<Type, Order, Alloc>;
		using map    = fwd::map<Type, Order, Alloc>;
		using span   = fwd::span<Type>;
		using vector = fwd::vector<Type, Alloc>;
		using graph  = fwd::graph<Type, Alloc>;
		using group  = fwd::group<Type, Alloc, Order>;
		using edges  = fwd::edges<Type>;
		using page   = fwd::page<Type, Alloc>;
	};

	template
	<
		class String,
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order,

		class Memory = memory<String>,
		class Stream = stream<Char, Traits>,
		class Layout = layout<String, Alloc, Order>
	>
	struct basic_string_type : String, Memory, Stream, Layout
	{
		using check  = typename Memory::check;
		using order  = typename Memory::order;
		using scan   = typename Stream::scan;
		using print  = typename Stream::print;
		using format = typename Stream::format;

		using String::String;
		basic_string_type(auto const& s)
		 : String(s)
		{ }
	};

	template
	<
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order,
		class String = fwd::basic_string<Char, Traits, Alloc>,
		class View = fwd::basic_string_view<Char, Traits>,
		class Base = basic_string_type<View , Char, Traits, Alloc, Order>
	>
	struct basic_string_view : Base
	{
		using Base::Base;
		basic_string_view(const String& in)
		: Base(in.data(), in.size())
		{ }
	};

	template
	<
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order,
		class String = fwd::basic_string<Char, Traits, Alloc>,
		class View = fwd::basic_string_view<Char, Traits>,
		class Base = basic_string_type<String, Char, Traits, Alloc, Order>
	>
	struct basic_string : Base
	{
		using view = basic_string_view<Char, Traits, Alloc, Order>;

		using Base::Base;
		basic_string(const View& in)
		: Base(in.data(), in.size())
		{ }
	};

	// low 7 bit UTF-8
	using string = basic_string<char>;
	using view = string::view;
	// UTF-16 or UTF-32 wide
	using wstring = basic_string<wchar_t>;
	using wide = wstring::view;
	// at least a UTF-32 code point
	using ustring = basic_string<wint_t>;
	using code = ustring::view;

	using tstring =
	#ifdef TCHAR
		basic_string<TCHAR>;
	#else
		basic_string<char>;
	#endif

	inline string::view empty("");
	inline string::view assign("=");

	constexpr auto eol = '\n';
	constexpr auto tab = '\t';
	constexpr auto nil = '\0';

	constexpr auto npos = string::npos;
	constexpr auto null = size_t { };

	static_assert(null == ~npos, "2's complement required");

	using size = fmt::layout<std::size_t>;
	using diff = fmt::layout<std::ptrdiff_t>;
}

#endif // file
