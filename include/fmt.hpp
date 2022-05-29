#ifndef fmt_hpp
#define fmt_hpp "Data Formats"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"

namespace fwd
{
	template
	<
		class Type, size_t Size = std::dynamic_extent, template <class> class Alloc = std::allocator
	>
	using matrix = vector<std::span<Type, Size>, Alloc>;
}

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
		using matrix = fwd::vector<span, Alloc>;
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
		using access = typename Stream::access;

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
	using iterator = view::iterator;
	using pointer = view::const_pointer;
	using vector = view::vector;
	using span = view::span;
	using map = view::map;
	using pair = view::pair;
	using set = view::set;
	using init = view::init;
	using input = view::input;
	using output = view::output;
	using read = view::read;
	using write = view::write;
	using access = view::access;
	// position
	using size_type = view::size_type;
	using size = fmt::layout<size_type>;
	using size_pair = size::pair;
	// offset
	using diff_type = view::difference_type;
	using diff = fmt::layout<diff_type>;
	using diff_pair = diff::pair;
	// storage
	using cache = string::set;
	using write_ptr = std::shared_ptr<cache>;
	using read_ptr = std::shared_ptr<const cache>;

	constexpr auto npos = view::npos;
	constexpr size_type null = 0;

	namespace tag
	{
		inline view empty = "";
		inline view eol = "\n";
		inline view tab = "\t";
		inline view dash = "-";
		inline view dual = "--";
		inline view assign = "=";
		inline view quote = "\"";

		bool got(view);
		view get(view);
		view put(view);
		view set(view);

		write_ptr write(); // copy to local on write
		read_ptr read(); // either local or main thread
		void sync(); // copy local back to main thread

		input get(input, view = eol);
		// Read all file lines to cache

		output put(output, view = eol);
		// Write all cache lines to file
	}
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
