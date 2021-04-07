// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "fmt.hpp"
#include "dig.hpp"
#include "str.hpp"
#include "type.hpp"
#include "char.hpp"
#include "sync.hpp"
#include "err.hpp"
#include <sstream>
#include <iomanip>
#include <charconv>
#include <system_error>
#include <cstdlib>
#include <cmath>

namespace
{
	class strings : fwd::unique
	{
		strings() = default;

		sys::exclusive<fmt::string::set> cache;
		sys::exclusive<fmt::view::vector> store;
		sys::exclusive<std::map<fmt::view, fmt::name>> table;

	public:

		bool got(fmt::name key)
		{
			fmt::name const index = ~key;
			fmt::name const size = store.read()->size();
			return -1 < index and index < size;
		}

		bool got(fmt::view key)
		{
			auto const reader = table.read();
			auto const it = reader->find(key);
			auto const end = reader->end();
			return end != it;
		}

		fmt::view get(fmt::name key)
		{
			auto const reader = store.read();
			auto const index = fmt::to_size(~key);
			#ifdef assert
			assert(got(key) and "String is not stored");
			#endif
			return reader->at(index);
		}

		fmt::view get(fmt::view key)
		{
			assert(not empty(key));
			// Lookup
			{
				auto const reader = table.read();
				auto const it = reader->find(key);
				auto const end = reader->end();
				if (end != it)
				{
					return it->first;
				}
			}
			// Create
			return get(set(key));
		}

		fmt::name put(fmt::view key)
		{
			assert(not empty(key));
			// Lookup
			{
				auto const reader = table.read();
				auto const end = reader->end();
				auto const it = reader->find(key);
				if (end != it)
				{
					return ~it->second;
				}
			}
			// Create
			auto writer = store.write();
			auto const size = writer->size();
			auto const id = fmt::to<fmt::name>(size);
			{
				auto [it, unique] = table.write()->emplace(key, id);
				assert(unique);
				writer->push_back(it->first);
			}
			return ~id;
		}

		fmt::name set(fmt::view key)
		{
			// Lookup
			{
				auto const reader = table.read();
				auto const end = reader->end();
				auto const it = reader->find(key);
				if (end != it)
				{
					return ~it->second;
				}
			}
			// Create
			auto writer = store.write();
			auto const size = writer->size();
			auto const id = fmt::to<fmt::name>(size);
			{
				// Cache the string here
				auto const p = cache.write()->emplace(key);
				verify(p.second);
				// Index a view to the string
				auto const q = table.write()->emplace(*p.first, id);
				verify(q.second);

				writer->push_back(q.first->first);
			}
			return ~id;
		}

		fmt::string::in::ref get(fmt::string::in::ref in, char end)
		{
			// Block all threads at this point
			auto wcache = cache.write();
			auto wstore = store.write();
			auto wtable = table.write();

			fmt::string line;
			while (std::getline(in, line, end))
			{
				auto const p = wcache->emplace(move(line));
				assert(p.second);

				auto const size = wstore->size();
				auto const id = fmt::to<fmt::name>(size);
				wstore->emplace_back(*p.first);

				auto const q = wtable->emplace(*p.first, id);
				assert(q.second);
			}
			return in;
		}

		fmt::string::out::ref put(fmt::string::out::ref out, char eol)
		{
			auto const reader = cache.read();
			auto const begin = reader->begin();
			auto const end = reader->end();

			for (auto it = begin; it != end; ++it)
			{
				out << *it << eol;
			}
			return out;
		}

		static auto& registry()
		{
			static strings singleton;
			return singleton;
		}
	};

	std::errc const noerr { };

	template <typename T, typename C>
	T to_fp(fmt::string::view u, T nan, C* cast)
	{
		char *it;
		auto ptr = u.data();
		auto const value = cast(ptr, &it);
		if (ptr == it)
		{
			sys::err(here, u);
			return nan;
		}
		return value;
	}

	template <typename T>
	T to_base(fmt::string::view u, int base)
	{
		auto begin = u.data();
		auto end = begin + u.size();
		auto value = static_cast<T>(std::nan(""));
		auto code = std::from_chars(begin, end, value, base);
		if (noerr != code.ec)
		{
			auto const error = std::make_error_code(code.ec);
			auto const message = error.message();
			sys::warn(here, message);
		}
		return value;
	}

	template <typename T>
	fmt::string from_base(T value, int base)
	{
		fmt::string s(20, '\0');
		do
		{
			auto begin = data(s);
			auto end = begin + size(s);
			auto code = std::to_chars(begin, end, value, base);
			if (noerr != code.ec)
			{
				if (std::errc::value_too_large == code.ec)
				{
					s.resize(2*s.size(), '\0');
					continue;
				}
				auto const error = std::make_error_code(code.ec);
				auto const message = error.message();
				sys::warn(here, message);
			}
			s.resize(code.ptr - begin);
		}
		while (0);
		s.shrink_to_fit();
		return s;
	}

	template <typename T>
	fmt::string from_fp(T value, int precision)
	{
		/* we somehow still have no compiler support
		fmt::string s(20, '\0');
		do
		{
			auto begin = s.data();
			auto end = begin + s.size();
			auto code = std::to_chars(begin, end, value, 0, precision);
			if (std::errc::value_too_large == code.ec)
			{
				s.resize(2*s.size(), '\0');
				continue;
			}
			s.resize(code.ptr - begin);
		}
		while (0);
		s.shrink_to_fit();
		return s;
		*/
		std::stringstream ss;
		ss << std::fixed << std::setprecision(precision) << value;
		return ss.str();
	}
}

namespace fmt
{
	template <class Char> type<Char> const & type<Char>::instance()
	{
		static type<Char> const local;
		return local;
	}

	template struct type<char>;
	template struct type<wchar_t>;

	string to_string(long value, int base)
	{
		return from_base<long>(value, base);
	}

	string to_string(long long value, int base)
	{
		return from_base<long long>(value, base);
	}

	string to_string(unsigned long value, int base)
	{
		return from_base<unsigned long>(value, base);
	}

	string to_string(unsigned long long value, int base)
	{
		return from_base<unsigned long long>(value, base);
	}

	string to_string(float value, int precision)
	{
		return from_fp<float>(value, precision);
	}

	string to_string(double value, int precision)
	{
		return from_fp<double>(value, precision);
	}

	string to_string(long double value, int precision)
	{
		return from_fp<long double>(value, precision);
	}

	long to_long(string::view u, int base)
	{
		return to_base<long>(u, base);
	}

	long long to_llong(string::view u, int base)
	{
		return to_base<long long>(u, base);
	}

	unsigned long to_ulong(string::view u, int base)
	{
		return to_base<unsigned long>(u, base);
	}

	unsigned long long to_ullong(string::view u, int base)
	{
		return to_base<unsigned long long>(u, base);
	}

	float to_float(string::view u)
	{
		auto const nan = std::nanf("");
		return to_fp(u, nan, std::strtof);
	}

	double to_double(string::view u)
	{
		auto const nan = std::nan("");
		return to_fp(u, nan, std::strtod);
	}

	long double to_quad(string::view u)
	{
		auto const nan = std::nanl("");
		return to_fp(u, nan, std::strtold);
	}

	bool got(name n)
	{
		return strings::registry().got(n);
	}

	bool got(view n)
	{
		return strings::registry().got(n);
	}

	view get(name n)
	{
		return strings::registry().get(n);
	}

	view get(view n)
	{
		return strings::registry().get(n);
	}

	name put(view n)
	{
		return strings::registry().put(n);
	}

	name set(view n)
	{
		return strings::registry().set(n);
	}

	string::in::ref get(string::in::ref in, char end)
	{
		return strings::registry().get(in, end);
	}

	string::out::ref put(string::out::ref out, char end)
	{
		return strings::registry().put(out, end);
	}
}

#ifdef test_unit

test_unit(dig)
{
	assert('*' == fmt::to_narrow<char>(42));
	assert(42u == fmt::to_unsigned(42));
	assert(42 == fmt::to_signed(42u));
	assert(42 == fmt::to_long("42"));
	assert(42 == fmt::to_long("52", 8));
	assert(42 == fmt::to_long("2A", 16));
	assert(0.42f == fmt::to_float("0.42f"));
	assert(std::isnan(fmt::to_float("nan")));
	assert(std::isinf(fmt::to_double("inf")));
	// catch
	//except(256 == (int) fmt::to_narrow<char>(256));
	//except(-1 == (signed) fmt::to_unsigned(-1));
	//except(0L == fmt::to_long("$"));
}

test_unit(type)
{
	fmt::string::view const Space = " \t\v\r\n\f";
	fmt::string::view const Hello = "Hello, World!";
	fmt::string::view const Upper = "HELLO, WORLD!";
	fmt::string::view const Lower = "hello, world!";
	fmt::wstring::view const Wide = L"Hello, World!";
	auto const Filled = fmt::to_string(Space) + fmt::to_string(Hello) + fmt::to_string(Space);

	// Character class/whitespace iteration
	{
		assert(fmt::next(Space) == Space.begin());
		assert(fmt::skip(Space) == Space.end());
		assert('H' == *fmt::first(Filled));
		assert('!' == *fmt::last(Filled));
	}

	// Triming whitespace
	{
		assert(std::empty(fmt::trim(Space)));
		assert(not std::empty(fmt::trim(Filled)));
		assert(Hello == fmt::trim(Filled));
	}

	// Split by whitespace/characters
	{
		fmt::string::view::vector const v
		{
			"1 2 3", " 1 2 3", "1 2 3 ", " 1 2 3 ",
			"\t1\n2\n\t3\t\n"
		};

		for (auto u : v)
		{
			auto const t = fmt::split(u);
			assert(t.size() == 3);
			assert(t.front() == "1");
			assert(t.back() == "3");
		}
	}

	// String view null terminator
	{
		assert(fmt::terminated(Hello));
		auto substr = Hello.substr(0, 5);
		assert(not fmt::terminated(substr));
	}

	// Character case conversion
	{
		assert(fmt::to_upper(Hello) == Upper);
		assert(fmt::to_lower(Hello) == Lower);
	}

	// Character encoding conversion
	{
		assert(fmt::to_wstring(Hello) == Wide);
		assert(fmt::to_string(Wide) == Hello);
	}

	// Search matching braces
	{
		using pos = std::pair<fmt::string::size_type, fmt::string::size_type>;

		assert(fmt::embrace("<A<B>C>", "<>") == pos { 0, 6 });
		assert(fmt::embrace("<A<B>C>", "<B>") == pos { 0, 3 });
		assert(fmt::embrace("A[B]C[D]", "[D]") == pos { 1, 3 });
		assert(fmt::embrace("{A<B}C}", "<>") == pos { 2, fmt::npos });
		assert(fmt::embrace("{A{B>C}", "<>") == pos { fmt::npos, fmt::npos });
		assert(fmt::embrace("&amp;", "&;") == pos { 0, 4 });
	}
}

test_unit(char)
{
	// Escape parameter encoding
	{
		fmt::string::stream ss;
		ss << fmt::io::par<1, 2, 3, 4>;
		assert(ss.str() == "1;2;3;4");
	}

	// Set graphics rendition
	{
		fmt::string::stream ss;
		ss << fmt::io::fg_green << "GREEN" << fmt::io::fg_off;
		assert(ss.str() == "\x1b[32mGREEN\x1b[39m");
	}
}

#endif
