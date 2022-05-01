// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "fmt.hpp"
#include "dig.hpp"
#include "tag.hpp"
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

namespace fmt
{
	template <class C> static type<C> singleton;
	template <class C> static auto address = &singleton<C>;

	template <class C> type<C>* type<C>::get()
	{
		return address<C>;
	}

	template <class C> type<C>* type<C>::set(type<C>* to)
	{
		return address<C> = to;
	}

	template <class C> bool type<C>::check(C c, mask x) const
	{
		return basic_type::is(x, c);
	}

	template <class C> typename type<C>::mark type<C>::check(view u) const
	{
		mark x(u.size());
		basic_type::is(u.data(), u.data() + u.size(), x.data());
		return x;
	}

	template <class C> typename type<C>::iterator type<C>::next(iterator it, iterator end, mask x) const
	{
		while (it != end)
		{
			if (check(*it, x))
			{
				break;
			}
			else ++it;
		}
		return it;
	}

	template <class C> typename type<C>::pointer type<C>::next(pointer it, pointer end, mask x) const
	{
		return basic_type::scan_is(x, it, end);
	}

	template <class C> typename type<C>::iterator type<C>::next(view u, mask x) const
	{
		return next(begin(u), end(u), x);
	}

	template <class C> typename type<C>::iterator type<C>::skip(iterator it, iterator end, mask x) const
	{
		while (it != end)
		{
			if (check(*it, x))
			{
				++it;
			}
			else break;
		}
		return it;
	}

	template <class C> typename type<C>::pointer type<C>::skip(pointer it, pointer end, mask x) const
	{
		return basic_type::scan_not(x, it, end);
	}

	template <class C> typename type<C>::iterator type<C>::skip(view u, mask x) const
	{
		return skip(u.begin(), u.end(), x);
	}

	template <class C> typename type<C>::vector type<C>::split(view u, mask x) const
	{
		vector t;
		auto const begin = u.data(), end = begin + u.size();
		for (auto i = skip(begin, end, x), j = end; i != end; i = skip(j, end, x))
		{
			j = next(i, end, x);
			auto const n = std::distance(i, j);
			t.emplace_back(i, n);
		}
		return t;
	}

	template <class C> typename type<C>::iterator type<C>::first(view u, mask x) const
	{
		return skip(u.begin(), u.end(), x);
	}

	template <class C> typename type<C>::iterator type<C>::last(view u, mask x) const
	{
		return skip(u.rbegin(), u.rend(), x).base() - 1;
	}

	template <class C> typename type<C>::view type<C>::trim(view u, mask x) const
	{
		auto const before = last(u, x) + 1;
		auto const after = first(u, x);
		auto const pos = std::distance(begin(u), after);
		auto const size = std::distance(after, before);
		return u.substr(pos, size);
	}

	template <class C> bool type<C>::all_of(view u, mask x) const
	{
		return fwd::all_of(widen(u), [this, x](auto w)
		{
			return this->check(w, x);
		});
	}

	template <class C> bool type<C>::any_of(view u, mask x) const
	{
		return fwd::any_of(widen(u), [this, x](auto w)
		{
			return this->check(w, x);
		});
	}

	template <class C> typename type<C>::string type<C>::to_upper(view u) const
	{
		string s;
		for (const auto w : widen(u))
		{
			const auto p = basic_type::toupper(w);
			s += from(p);
		}
		return s;
	}

	template <class C> typename type<C>::string type<C>::to_lower(view u) const
	{
		string s;
		for (const auto w : widen(u))
		{
			const auto p = basic_type::tolower(w);
			s += from(p);
		}
		return s;
	}

	template <class C> typename type<C>::pair type<C>::to_pair(view u, view v)
	{
		const auto m = u.size();
		const auto n = u.find(v);
		const auto p = u.substr(0, n);
		const auto q = u.substr(n < m ? n + 1 : m);
		return pair { p, q };
	}

	template <class C> bool type<C>::terminated(view u)
	{
		return not u.empty() and (u.back() == '\0' or u[u.size()] == '\0');
	}

		static auto count(view u, view v)
		// Count occurances in $u of a substring $v
		{
			auto n = null;
			auto const z = v.size();
			for (auto i = u.find(v); i != npos; i = u.find(v, i))
			{
				i += z;
				++ n;
			}
			return n;
		}

		template <class Iterator>
		static auto join(Iterator begin, Iterator end, view u)
		// Join strings in $t with $u inserted between
		{
			string s;
			for (auto it = begin; it != end; ++it)
			{
				if (begin != it)
				{
					s += u;
				}
				s += *it;
			}
			return s;
		}

		template <class Span> static auto split(Span t, view u)
		// Split strings in $p delimited by $v
		{
			fwd::vector<Span> s;
			auto p = t.data();
			size_t i = 0, j = 0;
			for (auto v : t)
			{
				if (u == v)
				{
					s.emplace_back(p + i, j - i);
					j = ++i;
				}
				else ++j;
			}
			return s;
		}

		static auto split(view u, view v)
		// Split strings in $u delimited by $v
		{
			vector t;
			auto const uz = u.size(), vz = v.size();
			for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
			{
				auto const k = uz < j ? uz : j;
				auto const w = u.substr(i, k - i);
				if (i <= k) t.emplace_back(w);
				i = k + vz;
			}
			return t;
		}

		static auto replace(view u, view v, view w)
		// Replace in u all occurrances of v with w
		{
			string s;
			auto const uz = u.size(), vz = v.size();
			for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
			{
				auto const k = std::min(j, uz);
				s = u.substr(i, k - i);
				if (j < uz) s += w;
				i = k + vz;
			}
			return s;
		}

		static auto embrace(view u, view v)
		// Position in u with matching braces front and back in v
		{
			auto i = u.find_first_of(v.front()), j = i;
			if (i < npos)
			{
				int n = 1;
				do
				{
					j = u.find_first_of(v, j + 1);
					if (npos == j)
					{
						break; // missing
					}
					else
					if (v.back() == u[j])
					{
						--n; // close
					}
					else
					if (v.front() == u[j])
					{
						++n; // open
					}
					else
					{
						break; // other
					}
				}
				while (0 < n);
			}
			return std::pair { i, j };
		}

	// global object linkage
	template struct type<char>;
	template struct type<wchar_t>;
}

namespace fmt::tag
{
	class strings : fwd::no_copy
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
				#ifdef assert
				assert(unique);
				#endif
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
				#ifdef assert
				assert(p.second);
				#endif

				// Index a view to the string
				auto const q = table.write()->emplace(*p.first, id);
				#ifdef assert
				assert(q.second);
				#endif

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

		fmt::string::out::ref put(fmt::string::out::ref out, char end)
		{
			auto const reader = cache.read();
			auto const begin = reader->begin();
			auto const end = reader->end();

			for (auto it = begin; it != end; ++it)
			{
				out << *it << end;
			}
			return out;
		}

		static auto & registry()
		{
			static strings instance;
			return instance;
		}
	};

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
			"1 2 3", " 1 2 3", "1 2 3 ", " 1 2 3 ",	"\t1\n2\n\t3\t\n"
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
	// Set graphics rendition
	{
		fmt::string::stream ss;
		ss << fmt::io::fg_green << "GREEN" << fmt::io::fg_off;
		assert(ss.str() == "\x1b[32mGREEN\x1b[39m");
	}
}

#endif
