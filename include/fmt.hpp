#ifndef fmt_hpp
#define fmt_hpp "Data FormatS"

#include "fwd.hpp"
#include "algo.hpp"
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
		using put = fwd::function<ref>;
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
		using input = typename in::ref;
		using output = typename out::ref;
		using read = typename in::put;
		using write = typename out::put;
		using address = typename addr::cptr;

		template <class Iterator>
		static output put(output buf, Iterator begin, Iterator end, address tok)
		// Put $tok from $begin to $end in $buf
		{
			auto it = std::ostream_iterator(buf, tok);
			std::copy(begin, end, it);
			return buf;
		}

		template <class Iterator>
		static output put(output out, Iterator begin, Iterator end, write tok)
		// Put $tok from $begin to $end in $buf
		{
			for (auto it = begin; it != end; ++it)
				(it == begin ? out : out << tok) << *it;
			return out;
		}

		template <class Range, class Delimiter>
		static output put(output buf, Range pair, Delimiter tok)
		// Put $tok between all in range of $pair
		{
			return put(buf, pair.begin(), pair.end(), tok);
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
		using line = fwd::line<Char, Traits, Alloc>;
		using page = fwd::page<String, Alloc>;

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
		template <class> class Sort = std::less,
		class String = fwd::basic_string<Char, Traits, Alloc>,
		class View = fwd::basic_string_view<Char, Traits>,
		class Base = basic_string_type<View , Char, Traits, Alloc, Sort>
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
	using uint = ustring::view;

	// alias
	using pair = view::pair;
	using vector = view::vector;
	using span = view::span;
	using map = view::map;
	using init = view::init;
	using set = view::set;
	using input = view::input;
	using output = view::output;
	using read = view::read;
	using write = view::write;
	using page = view::page;
	using line = view::line;

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

#endif // file
