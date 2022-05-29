// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "type.hpp"
#include "char.hpp"
#include "meta.hpp"
#include <sstream>
#include <iomanip>
#include <charconv>
#include <iostream>
#include <system_error>
#include <cstdlib>
#include <cmath>

namespace doc
{
	template struct instance<fmt::view>;
	using view = instance<fmt::view>;
}

namespace fmt::tag
{
	static sys::exclusive<fmt::cache> cache;

	view put(view u)
	{
		auto key = cache.writer();
		const auto begin = key->begin();
		const auto end = key->end();
		auto it = std::find_if(begin, end, fwd::equal_to(u));
		if (end == it)
		{
			auto pair = key->emplace(u);
			#ifdef assert
			assert(pair.second);
			#endif
			it = pair.first;
		}
		return it->substr();
	}

	input get(input in, char eol)
	{
		fmt::string line;
		while (std::getline(in, line, eol))
		{
			(void) put(line);
		}
		return in;
	}

	output set(output out, char eol)
	{
		auto key = cache.reader();
		const auto begin = key->begin();
		const auto end = key->end();
		for (auto it = begin; it != end; ++it)
		{
			out << *it << eol;
		}
		return out;
	}
}

namespace
{
	template <class Face> const Face& use_facet()
	{
		return std::use_facet<Face>(std::cout.getloc());
	}

	template <class Char, class Type> auto as_string(const Type& p)
	{
		if constexpr (std::is_same<Char, char>::value)
		{
			return fmt::to_string(p);
		}
		else
		{
			return fmt::to_wstring(p);
		}
	}
}

namespace fmt
{
	template <class C> const typename type<C>::ctype* type<C>::use_ctype()
	{
		return & use_facet<ctype>();
	}

	template <class C> size_type type<C>::length(C u)
	// Size of UTF encoded data from first item
	{
		size_type n = 1;
		if (std::signbit(u))
		{
			for (n = 0; std::signbit(u << n); ++n);
		}
		return n;
	}

	template <class C> bool type<C>::check(C c, mask x)
	{
		return use_facet<ctype>().is(x, c);
	}

	template <class C> mark type<C>::check(view u)
	{
		mark x(u.size());
		use_facet<ctype>().is(u.data(), u.data() + u.size(), x.data());
		return x;
	}

	template <class C> template <class It> It type<C>::scan_is(It it, It end, mask x)
	{
		if constexpr (std::is_same<It, pointer>::value)
		{
			return use_facet<ctype>().scan_is(x, it, end);
		}
		else
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
	}

	template <class C> template <class It> It type<C>::scan_not(It it, It end, mask x)
	{
		if constexpr (std::is_same<It, pointer>::value)
		{
			return use_ctype()->scan_not(x, it, end);
		}
		else
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
	}

	template <class C> typename type<C>::iterator type<C>::next(iterator it, iterator end, mask x)
	{
		return scan_is(it, end, x);
	}

	template <class C> typename type<C>::iterator type<C>::next(view u, mask x)
	{
		return next(u.begin(), u.end(), x);
	}

	template <class C> typename type<C>::iterator type<C>::skip(iterator it, iterator end, mask x)
	{
		return scan_not(it, end, x);
	}

	template <class C> typename type<C>::iterator type<C>::skip(view u, mask x)
	{
		return skip(u.begin(), u.end(), x);
	}

	template <class C> typename type<C>::vector type<C>::split(view u, mask x)
	{
		vector t;
		const auto begin = u.data(), end = begin + u.size();
		for (auto i = skip(begin, end, x), j = end; i != end; i = skip(j, end, x))
		{
			j = next(i, end, x);
			const auto n = std::distance(i, j);
			t.emplace_back(i, n);
		}
		return t;
	}

	template <class C> typename type<C>::iterator type<C>::first(view u, mask x)
	{
		return scan_is(u.begin(), u.end(), x);
	}

	template <class C> typename type<C>::iterator type<C>::last(view u, mask x)
	{
		return scan_is(u.rbegin(), u.rend(), x).base() - 1;
	}

	template <class C> typename type<C>::view type<C>::trim(view u, mask x)
	{
		const auto before = last(u, x) + 1;
		const auto after = first(u, x);
		const auto pos = std::distance(u.begin(), after);
		const auto size = std::distance(after, before);
		return u.substr(pos, size);
	}

	template <class C> typename type<C>::view type<C>::trim(view u, view v)
	{
		const auto before = u.find_first_not_of(v);
		const auto after = u.find_last_of(v);
		return u.substr(before, after);
	}

	template <class C> bool type<C>::all_of(view u, mask x)
	{
		return fwd::all_of(widen(u), [x](auto w)
		{
			return check(w, x);
		});
	}

	template <class C> bool type<C>::any_of(view u, mask x)
	{
		return fwd::any_of(widen(u), [x](auto w)
		{
			return check(w, x);
		});
	}

	template <class C> typename type<C>::string type<C>::to_upper(view u)
	{
		string s;
		const auto use = use_ctype();
		for (const auto w : widen(u))
		{
			const auto p = use->toupper(w);
			s += as_string<C>(p);
		}
		return s;
	}

	template <class C> typename type<C>::string type<C>::to_lower(view u)
	{
		string s;
		const auto use = use_ctype();
		for (const auto w : widen(u))
		{
			const auto p = use->tolower(w);
			s += as_string<C>(p);
		}
		return s;
	}

	template <class C> typename type<C>::pair type<C>::to_pair(view u, view v)
	{
		const auto m = u.size();
		const auto n = u.find(v);
		const auto p = u.substr(0, n);
		const auto q = u.substr(n < m ? n + 1 : m);
		return { p, q };
	}

	template <class C> bool type<C>::terminated(view u)
	{
		return not u.empty() and (u.back() == null or u[u.size()] == null);
	}

	template <class C> size_type type<C>::count(view u, view v)
	{
		auto n = null;
		const auto z = v.size();
		for (auto i = u.find(v); i != npos; i = u.find(v, i))
		{
			i += z;
			++ n;
		}
		return n;
	}

	template <class C> typename type<C>::string type<C>::join(span t, view u)
	{
		string s;
		const auto begin = t.begin();
		const auto end = t.end();
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

	template <class C> typename type<C>::vector type<C>::split(view u, view v)
	{
		vector t;
		const auto uz = u.size(), vz = v.size();
		for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
		{
			const auto k = uz < j ? uz : j;
			const auto w = u.substr(i, k - i);
			if (i <= k) t.emplace_back(w);
			i = k + vz;
		}
		return t;
	}

	template <class C> typename type<C>::string type<C>::replace(view u, view v, view w)
	{
		string s;
		const auto uz = u.size(), vz = v.size();
		for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
		{
			const auto k = std::min(j, uz);
			s = u.substr(i, k - i);
			if (j < uz) s += w;
			i = k + vz;
		}
		return s;
	}

	template <class C> size_pair type<C>::embrace(view u, view v)
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
		return { i, j };
	}

	template <class C> C type<C>::getline(input in, string& line, view delims)
	{
		C byte;
		while (in.get(byte) and delims.find(byte) < fmt::npos)
		{
			line += byte;
		}
		return byte;
	}

	template struct type<char>;
	template struct type<wchar_t>;
}


#ifdef test_unit

test_unit(type)
{
	const fmt::view Space = " \t\v\r\n\f";
	const fmt::view Hello = "Hello, World!";
	const fmt::view Upper = "HELLO, WORLD!";
	const fmt::view Lower = "hello, world!";
	const fmt::wide Wide = L"Hello, World!";
	const auto Filled = fmt::to_string(Space) + fmt::to_string(Hello) + fmt::to_string(Space);

	// Character class/whitespace iteration
	{
		assert(fmt::next(Space) == Space.begin());
		assert(fmt::skip(Space) == Space.end());
		assert('H' == *fmt::first(Filled));
		assert('!' == *fmt::last(Filled));
	}

	// Trimming whitespace
	{
		assert(fmt::trim(Space).empty());
		assert(not fmt::trim(Filled).empty());
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
		using pos = fmt::size::pair;

		assert(fmt::embrace("<A<B>C>", "<>") == pos { 0, 6 });
		assert(fmt::embrace("<A<B>C>", "<B>") == pos { 0, 3 });
		assert(fmt::embrace("A[B]C[D]", "[D]") == pos { 1, 3 });
		assert(fmt::embrace("{A<B}C}", "<>") == pos { 2, -1 });
		assert(fmt::embrace("{A{B>C}", "<>") == pos { -1, -1 });
		assert(fmt::embrace("&amp;", "&;") == pos { 0, 4 });
	}
}

test_unit(sgr)
{
	std::stringstream ss;
	ss << fmt::io::fg_green << "GREEN" << fmt::io::fg_off;
	auto green = ss.str();
	assert(green == "\x1b[32mGREEN\x1b[39m");
}

test_unit(fmt)
{
	constexpr auto msg = "Hello, World!";
	fmt::write write = [=](fmt::output out)->fmt::output
	{
		return out << msg;
	};
	std::stringstream ss;
	ss << write;
	const auto s = ss.str();
	assert(s == msg);
}

#endif
