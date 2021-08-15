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
