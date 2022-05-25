#ifndef fmt_hpp
#define fmt_hpp "Data Formats"

#include "fwd.hpp"
#include "tmp.hpp"

namespace fmt
{
	template <class Type> struct memory
	{
		// address traits
		using ptr = typename std::add_pointer<Type>::type;
		using cptr = typename std::add_pointer<typename std::add_const<Type>::type>::type;
		using ref = typename std::add_lvalue_reference<Type>::type;
		using cref = typename std::add_lvalue_reference<typename std::add_const<Type>::type>::type;

		// access traits
		using access = fwd::function<ref>;
		using test = fwd::predicate<cref>;
		using cache = fwd::predicate<ref>;
		using order = fwd::relation<cref>;
		using swap = fwd::relation<ref>;
	};

	template
	<
		class Char, template <class> class Traits = std::char_traits
	>
	struct stream
	{
		// address traits
		using addr = memory<Char>;
		using ios = memory<fwd::basic_ios<Char, Traits>>;
		using in = memory<fwd::basic_istream<Char, Traits>>;
		using out = memory<fwd::basic_ostream<Char, Traits>>;
		using io = memory<fwd::basic_iostream<Char, Traits>>;
		using buf = memory<fwd::basic_buf<Char, Traits>>;
		using file = memory<fwd::basic_file<Char, Traits>>;
		using str = memory<fwd::basic_stringstream<Char, Traits>>;

		// access traits
		using address = typename addr::cptr;
		using input = typename in::ref;
		using output = typename out::ref;
		using read = typename in::access;
		using write = typename out::access;
		using access = fwd::pair<read, write>;

		template <class Iterator>
		static output put(output out, Iterator begin, Iterator end, address tok)
		{
			auto it = std::ostream_iterator(out, tok);
			std::copy(begin, end, it);
			return out;
		}

		template <class Iterator>
		static output put(output out, Iterator begin, Iterator end, write tok)
		{
			for (auto it = begin; it != end; ++it)
				(it == begin ? out : out << tok) << *it;
			return out;
		}

		template <class Range, class Delimiter>
		static output put(output out, Range pair, Delimiter tok)
		{
			return put(out, pair.begin(), pair.end(), tok);
		}
	};

	template
	<
		class Type,
		template <class> class Alloc = std::allocator,
		template <class> class Order = std::less
	>
	struct layout
	{
		using type = Type;
		using pair = fwd::pair<Type>;
		using set = fwd::set<Type, Order, Alloc>;
		using map = fwd::map<Type, Type, Order, Alloc>;
		using span = fwd::span<Type>;
		using vector = fwd::vector<Type, Alloc>;
		using init = fwd::init<Type>;
	};

	template
	<
		class String,
		class Char,
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator,
		template <class> class Order = std::less,
		class Memory = memory<String>,
		class Stream = stream<Char, Traits>,
		class Layout = layout<String, Alloc, Order>
	>
	struct basic_string_type : String, Memory, Stream, Layout
	{
		using String::String;
		basic_string_type(const String& s)
		 : String(s)
		{ }
	};

	template
	<
		class Char,
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator,
		template <class> class Order = std::less,
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
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator,
		template <class> class Sort = std::less,
		class String = fwd::basic_string<Char, Traits, Alloc>,
		class View = fwd::basic_string_view<Char, Traits>,
		class Base = basic_string_type<String, Char, Traits, Alloc, Sort>
	>
	struct basic_string : Base
	{
		using view = basic_string_view<Char, Traits, Alloc, Sort>;

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
	using ustring = basic_string<char32_t>;
	using page = ustring::view;

	// everything is a string view in UTF
	using pair = view::pair;
	using iterator = view::iterator;
	using vector = view::vector;
	using span = view::span;
	using map = view::map;
	using init = view::init;
	using set = view::set;
	using input = view::input;
	using output = view::output;
	using read = view::read;
	using write = view::write;
	using access = fwd::pair<read, write>;

	namespace tag
	{
		inline view empty = "";
		inline view eol = "\n";
		inline view tab = "\t";
		inline view dash = "-";
		inline view dual = "--";
		inline view assign = "=";
	}

	using size = layout<std::size_t>;
	using diff = layout<std::ptrdiff_t>;
	constexpr auto npos = view::npos;
	constexpr size::type null = 0;
}

inline fmt::input operator>>(fmt::input in, fmt::read scan)
{
	return scan(in);
}

inline fmt::output operator<<(fmt::output out, fmt::write print)
{
	return print(out);
}

inline fmt::input operator>>(fmt::input in, fmt::access ops)
{
	return ops.first(in);
}

inline fmt::output operator<<(fmt::output out, fmt::access ops)
{
	return ops.second(out);
}

#endif // file
