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

		using next  = std::function<cref(cref)>;
		using put   = std::function<ref(ref)>;
		using get   = std::function<cref()>;
		using set   = std::function<ref()>;
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

		using input  = in::put;
		using output = out::put;
		using format = ctype::get;
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
	struct basic_string_type : fwd::compose<String, Memory, Stream, Layout>
	{
		using check = String::check;
		using order = String::order;
		using format = Stream::format;
		using input = Stream::input;
		using output = Stream::output;

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

		class View = fwd::basic_string_view<Char, Traits>
	>
	struct basic_string_view : basic_string_type<View , Char, Traits, Alloc, Order>
	{
		using View::View;
		basic_string_view(auto const& in)
		: View(data(in), size(in))
		{ }
	};

	template
	<
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order,

		class String = fwd::basic_string<Char, Traits, Alloc>
	>
	struct basic_string : basic_string_type<String, Char, Traits, Alloc, Order>
	{
		using view = basic_string_view<Char, Traits, Alloc, Order>;

		using String::String;
		basic_string(auto const& in)
		: String(data(in), size(in))
		{ }
	};

	// low 7 bit UTF-8
	using bstring = basic_string<char>;
	// UTF-16 or UTF-32 wide
	using wstring = basic_string<wchar_t>;
	// at least a UTF-32 code point
	using ustring = basic_string<wint_t>;

	using tstring =
	#ifdef TCHAR
		basic_string<TCHAR>;
	#else
		basic_string<char>;
	#endif

	using string = bstring;
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
