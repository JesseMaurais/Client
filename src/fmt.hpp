#ifndef fmt_hpp
#define fmt_hpp "Standard Format"

#include "fwd.hpp"
#include "algo.hpp"

namespace fmt
{
	template
	<
		class Type
	>
	struct memory_brief
	{
		using ptr  = typename std::add_pointer<Type>::type;
		using cptr = typename std::add_pointer<typename std::add_const<Type>::type>::type;
		using ref  = typename std::add_lvalue_reference<Type>::type;
		using cref = typename std::add_lvalue_reference<typename std::add_const<Type>::type>::type;
	};

	template
	<
		class Char, template <class> class Traits = fwd::character
	>
	struct stream_brief
	{
		using ios  = memory_brief<fwd::basic_ios<Char, Traits>>;
		using in   = memory_brief<fwd::basic_istream<Char, Traits>>;
		using out  = memory_brief<fwd::basic_ostream<Char, Traits>>;
		using io   = memory_brief<fwd::basic_iostream<Char, Traits>>;
		using buf  = memory_brief<fwd::basic_buf<Char, Traits>>;
		using file = memory_brief<fwd::basic_file<Char, Traits>>;
		using str  = memory_brief<fwd::basic_stringstream<Char, Traits>>;
	};

	template
	<
		class Type, 
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order
	>
	struct struct_brief : memory_brief<Type>
	{
		using type   = Type;
		using pair   = std::pair<Type, Type>;
		using init   = fwd::init<Type>;
		using set    = fwd::set<Type, Order, Alloc>;
		using map    = fwd::map<Type, Order, Alloc>;
		using span   = fwd::span<Type>;
		using vector = fwd::vector<Type, Alloc>;
		using graph  = fwd::graph<Type, Alloc>;
		using group  = fwd::group<Type, Order, Alloc>;
		using edges  = fwd::edges<Type>;
		using line   = fwd::line<Type>;
	};

	template
	<
		class String,
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order,
		// base class composition
		class Base = fwd::compose
		<
			struct_brief<String, Alloc, Order>, stream_brief<Char, Traits>
		>
	>
	struct string_brief : String, Base
	{ 
		using String::String;
		using stream = fwd::basic_stringstream<Char, Traits, Alloc>;

		string_brief(String const& s) : String(s)
		{ }
	};

	template
	<
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order,
		// details
		class Base = string_brief
		<
			fwd::basic_string_view<Char, Traits>, Char, Traits, Alloc, Order
		>
	>
	struct basic_string_view : Base
	{
		basic_string_view(fwd::basic_string<Char, Traits, Alloc> const& in)
		: Base(data(in), size(in))
		{ }

		using Base::Base;
	};

	template
	<
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class> class Order = fwd::order,
		// base class composition
		class Base = string_brief
		<
			fwd::basic_string<Char, Traits>, Char, Traits, Alloc, Order
		>
	>
	struct basic_string : Base
	{
		using view = basic_string_view<Char, Traits, Alloc, Order>;

		basic_string(fwd::basic_string_view<Char, Traits> in)
		: Base(data(in), size(in))
		{ }

		basic_string(fwd::basic_string<Char, Traits, Alloc> const& in)
		: Base(data(in), size(in))
		{ }

		using Base::Base;
	};

	// low 7 bit UTF-8
	using bstring = basic_string<char>;
	// UTF-16 or UTF-32 wide
	using wstring = basic_string<wchar_t>;
	// at least a UTF-32 code point
	using ustring = basic_string<wint_t>;

	using string =
	#ifdef _T
		basic_string<_T>;
	#else
		basic_string<char>;
	#endif
	using view = string::view;
	using line = string::line;

	inline view empty("");
	inline view assign("=");

	constexpr auto eol = '\n';
	constexpr auto tab = '\t';
	constexpr auto nil = '\0';

	constexpr auto npos = string::npos;
	constexpr auto null = size_t { };

	static_assert(null == ~npos, "Need two's complement");
}

#endif // file
