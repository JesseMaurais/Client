#ifndef fmt_hpp
#define fmt_hpp "Data Formats"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"

namespace fmt
{
	template <class Type> struct memory : fwd::type_of<Type>
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
		using compare = fwd::relation<cref>;
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
		using view = memory<fwd::basic_string_view<Char, Traits>>;

		// access traits
		using address = typename addr::cptr;
		using buffer = typename str::type;
		using test = typename view::test;
		using compare = typename view::compare;
		using binary = typename io::ref;
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
		static output put(output out, Range pair, Delimiter token)
		{
			return put(out, pair.begin(), pair.end(), token);
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
		using shared = fwd::shared<Type>;
		using pair = fwd::pair<Type>;
		using set = fwd::set<Type, Order, Alloc>;
		using map = fwd::map<Type, Type, Order, Alloc>;
		using init = fwd::init<Type>;
		using vector = fwd::vector<Type, Alloc>;
		using span = fwd::span<Type>;
		using matrix = fwd::matrix<Type, Alloc>;
		using base = fwd::base<Type>;
		using param = fwd::param<Type>;
		using edges = fwd::edges<Type, Alloc>;
		using div = fwd::pair<vector>;
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
		using test = typename Stream::test;
		using stream = typename Stream::str;
		using compare = typename Stream::compare;
		using iterator = typename String::iterator;
		using size_type = typename String::size_type;

		auto sub(iterator it, iterator end = String::end())
		{
			const auto pos = std::distance(String::begin(), it);
			const auto size = std::distance(it, end);
			const auto ptr = String::data() + pos;
			return basic_string_type(ptr, size);
		}

		auto before(iterator it)
		{
			return sub(String::begin(), it);
		}

		auto after(iterator it)
		{
			const auto end = String::end();
			return sub(it == end ? end : std::next(it));
		}

		auto split(iterator it)
		{
			return std::pair { before(it), after(it) };
		}

		auto split(size_type pos)
		{
			const auto it = String::npos == pos ? String::end() : std::next(String::begin(), pos);
			return split(it);
		}

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
	using buffer = view::buffer;
	using address = view::address;
	using iterator = view::iterator;
	using pointer = view::const_pointer;
	using vector = view::vector;
	using span = view::span;
	using matrix = view::matrix;
	using base = view::base;
	using edges = view::edges;
	using div = view::div;
	using map = view::map;
	using pair = view::pair;
	using set = view::set;
	using init = view::init;
	using param = view::param;
	using binary = view::binary;
	using input = view::input;
	using output = view::output;
	using read = view::read;
	using write = view::write;
	using access = view::access;
	using test = view::test;
	using compare = view::compare;
	using cache = string::set;
	// position
	using size_type = view::size_type;
	using size = fmt::layout<size_type>;
	using size_pair = size::pair;
	// offset
	using diff_type = view::difference_type;
	using diff = fmt::layout<diff_type>;
	using diff_pair = diff::pair;

	constexpr auto npos = view::npos;
	constexpr size_type null = 0;

	namespace tag
	{
		inline view empty = "";
		inline view dot = ".";
		inline view dots = "..";
		inline view tab = "\t";
		inline view eol = "\n";
		inline view slash = "/"; 
		inline view pass = "//";
		inline view dash = "-";
		inline view dual = "--";
		inline view quote = "\"";
		inline view assign = "=";

		view emplace(view);

		input read(input, view = eol);
		output write(output, view = eol);
	}
}

inline fmt::input operator>>(fmt::input in, fmt::read read)
{
	return read(in);
}

inline fmt::output operator<<(fmt::output out, fmt::write write)
{
	return write(out);
}

inline fmt::input operator>>(fmt::input in, fmt::access access)
{
	return access.first(in);
}

inline fmt::output operator<<(fmt::output out, fmt::access access)
{
	return access.second(out);
}

#endif // file
